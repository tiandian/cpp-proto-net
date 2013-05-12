#include "MdSpi.h"
#include "QS_Configuration.h"
#include "SubscribeQuoteObj.h"

#include <boost/interprocess/shared_memory_object.hpp>  
#include <boost/interprocess/mapped_region.hpp>  
#include <boost/interprocess/sync/scoped_lock.hpp> 

#ifndef WIN32
#define strcpy_s strcpy
#define _strdup strdup
#endif

using namespace boost::interprocess;

extern CQSConfiguration qsConfig;

CMdSpi::CMdSpi(CThostFtdcMdApi* pUserApi)
	: m_pUserApi(pUserApi)
	, m_iRequestId(0)
{
}

CMdSpi::~CMdSpi(void)
{
}

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cout << "--->>> " << "OnHeartBeatWarning" << endl;
	cout << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cout << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, qsConfig.BrokerId().c_str());
	strcpy_s(req.UserID, qsConfig.Username().c_str());
	strcpy_s(req.Password, qsConfig.Password().c_str());
	int iResult = m_pUserApi->ReqUserLogin(&req, ++m_iRequestId);
	cout << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cout << "--->>> 获取当前交易日 = " << m_pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		m_thSubscribe = boost::thread(boost::bind(&CMdSpi::SubscribeMarketData, this));	
	}
}

// Following is only for test
char *ppInstrumentID[] = { "IF1305" };			// 行情订阅列表
int iInstrumentID = 1;	

void CMdSpi::SubscribeMarketData()
{
	string shmName = "SubscribeQuote-" + qsConfig.BrokerId() + "-" + qsConfig.Username();

	cout << "Open memory object: " << shmName << endl;
	//Create a shared memory object.  
	shared_memory_object shm  
		(open_only                    //only create  
		,shmName.c_str()              //name  
		,read_write                   //read-write mode  
		);  

	try{  
		//Map the whole shared memory in this process  
		mapped_region region  
			(shm                       //What to map  
			,read_write //Map it as read-write  
			);  
		cout << "get region address" << endl;
		//Get the address of the mapped region  
		void * addr       = region.get_address();  

		//Obtain a pointer to the shared structure  
		SubscribeQuoteObj * data = static_cast<SubscribeQuoteObj*>(addr);  

		//Print messages until the other process marks the end  
		bool end_loop = false;  
		do{  
			scoped_lock<interprocess_mutex> lock(data->mutex);
			cout << "wait for condition of submit" << endl;
			data->cond_submit.wait(lock);
			cout << "begin subscribing/unsubscribing" << endl;
			if(data->running)
			{
				char** symbols = new char*[1];
				symbols[0] = data->items[0];

				if(data->subscribe)
				{
					cout << "Subscribing " << symbols[0] << endl;
					int iResult = m_pUserApi->SubscribeMarketData(symbols, 1);
					cout << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
				}
				else
				{
					cout << "Unsubscribing " << symbols[0] << endl;
					int iResult = m_pUserApi->UnSubscribeMarketData(symbols, 1);
					cout << "--->>> 发送行情退订请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
				}

				data->clear();
				data->message_in = false;
				data->cond_ready.notify_one();
			}
			else
				end_loop = true;
		}  
		while(!end_loop);
	}  
	catch(interprocess_exception &ex){  
		std::cout << ex.what() << std::endl;  
		return;  
	}  

	cout << "Proc SubscribeMarketData exit." << endl;
	//int iResult = m_pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	//cout << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspSubMarketData - " << pSpecificInstrument->InstrumentID << 
		(pRspInfo->ErrorID == 0 ? " Succeeded" : " Failed") << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspUnSubMarketData - " << pSpecificInstrument->InstrumentID << 
		(pRspInfo->ErrorID == 0 ? " Succeeded" : " Failed") << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cout << "OnRtnDepthMarketData : " << pDepthMarketData->InstrumentID << ", "
		<< pDepthMarketData->LastPrice << ", "
		<< pDepthMarketData->UpdateTime << ", "
		<< pDepthMarketData->UpdateMillisec << endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}
