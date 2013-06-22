#include "StdAfx.h"
#include "MdSpi.h"
#include "QS_Configuration.h"

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
	, m_loginWaiter(pUserApi)
	, m_iRequestId(0)
	, m_exitCode(-1)
{
	// Wait Login success for 10 seconds 
	m_loginWaiter.BeginWait(10);
}

CMdSpi::~CMdSpi(void)
{
	cout << "CMdSpi destructing..." << endl;
	m_pUserApi = NULL;
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
		m_loginWaiter.Cancel();

		///获取当前交易日
		cout << "--->>> 获取当前交易日 = " << m_pUserApi->GetTradingDay() << endl;

		string shmName = SHM_SUBSCRIBE_NAME + qsConfig.BrokerId() + "-" + qsConfig.Username();
		cout << "Opening shm " << shmName << " for quote subscribe" << endl;
		m_quoteSubscriber = boost::shared_ptr<CShmQuoteSubscribeConsumer>
			( new CShmQuoteSubscribeConsumer(shmName,
				boost::bind(&CMdSpi::SubscribeMarketData, this, _1, _2),
				boost::bind(&CMdSpi::UnsubscribeMarketData, this, _1, _2),
				boost::bind(&CMdSpi::OnTerminateNotified, this)));
		bool initSucc = m_quoteSubscriber->Init();
		if(!initSucc)
		{
			cout << "[QuoteStation] Quote subscriber initializtion failed" << endl;
			return;
		}

		string quoteFeedName = SHM_QUOTE_FEED_NAME + qsConfig.BrokerId() + "-" + qsConfig.Username();
		cout << "Open shm " << quoteFeedName << " for quote feeding" << endl;
		m_quoteFeeder = boost::shared_ptr<CShmQuoteFeedProducer>( new CShmQuoteFeedProducer(quoteFeedName));
		initSucc = m_quoteFeeder->Init();
		if(!initSucc)
		{
			cout << "[QuoteStation] Quote feeder initializtion failed" << endl;
			return;
		}
		
		m_quoteSubscriber->Start();
	}
}

void CMdSpi::SubscribeMarketData( char** symbolArr, int symCount )
{
	if(symbolArr != NULL && symCount > 0)
	{
		cout << "Subscribing " << symCount << " symbol(s). The first is " << symbolArr[0] << endl;
		int iResult = m_pUserApi->SubscribeMarketData(symbolArr, symCount);
		cout << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
	}
}

void CMdSpi::UnsubscribeMarketData( char** symbolArr, int symCount )
{
	if(symbolArr != NULL && symCount > 0)
	{
		cout << "Unsubscribing " << symCount << " symbol(s). The first is " << symbolArr[0] << endl;
		int iResult = m_pUserApi->UnSubscribeMarketData(symbolArr, symCount);
		cout << "--->>> 发送行情退订请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
	}
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
	//cout << "CMdSpi::OnRtnDepthMarketData : " << pDepthMarketData->InstrumentID << ", "
	//	<< pDepthMarketData->LastPrice << ", "
	//	<< pDepthMarketData->UpdateTime << ", "
	//	<< pDepthMarketData->UpdateMillisec << endl;

	m_quoteFeeder->Put(pDepthMarketData);
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void CMdSpi::OnTerminateNotified()
{
	if(m_pUserApi != NULL)
	{
		m_exitCode = 0; 
		m_pUserApi->Release();
	}
}
