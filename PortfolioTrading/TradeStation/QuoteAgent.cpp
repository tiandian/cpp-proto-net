#include "StdAfx.h"
#include "QuoteAgent.h"
#include "globalmembers.h"
#include "FileOperations.h"

#include <boost/format.hpp>

#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")

#define SYMBOL_MAX_LENGTH 10
#define CONNECT_TIMEOUT_SECONDS 15
#define DISCONNECT_TIMEOUT_SECOND 5

CQuoteAgent::CQuoteAgent(void)
{
}


CQuoteAgent::~CQuoteAgent(void)
{
}

void RunMarketDataFunc(CThostFtdcMdApi* pUserApi, const char* address)
{
	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(strdup(address));

	pUserApi->RegisterFront(front_addr.get());					// connect
	pUserApi->Init();

	pUserApi->Join();
}

boost::tuple<bool, string> CQuoteAgent::Open( const string& address, const string& streamDir )
{
	string errMsg;
	try{
		string streamFolder = streamDir + "\\Md";
		if(!CreateFolderIfNotExists(streamFolder))
		{
			errMsg = boost::str(boost::format("Cannot create stream folder (%s)") % streamFolder);
			return boost::make_tuple(false, errMsg);
		}
		streamFolder += "\\";
		// 初始化UserApi
		m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi(streamFolder.c_str());			// 创建UserApi
		m_pUserApi->RegisterSpi(this);						// 注册事件类

		logger.Info(boost::str(boost::format("Try to connect market (%s) ...") % address));

		m_thQuoting = boost::thread(&RunMarketDataFunc, m_pUserApi, address.c_str());

		// wait 1 minute for connected event
		{
			boost::unique_lock<boost::mutex> lock(m_mutex);
			if(!m_condConnectDone.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				errMsg = "Connecting time out";
				logger.Warning(errMsg);
				return boost::make_tuple(false, errMsg);
			}

			m_bIsConnected = true;
		}
		return boost::make_tuple(true, errMsg);
	}
	catch(std::exception& ex)
	{
		errMsg = "Failed to connect to market for quote, details is following\n";
		errMsg = ex.what();
		logger.Error(errMsg);
	}
	catch(...)
	{
		errMsg = "Unknown error encounted while connecting market for quote";
		logger.Error(errMsg);
	}

	return boost::make_tuple(false, errMsg);
}

void CQuoteAgent::Close()
{
	if(!m_bIsConnected)
		return;

	if(m_pUserApi != NULL)
	{
		//m_pUserApi->RegisterSpi(NULL);
		m_pUserApi->Release();

		{
			boost::unique_lock<boost::mutex> lock(m_mutex);

			if(m_bIsConnected)
			{
				if(!m_condConnectDone.timed_wait(lock, boost::posix_time::seconds(DISCONNECT_TIMEOUT_SECOND)))
				{
					logger.Warning("Disconnecting time out");
					m_bIsConnected = false;
				}
			}
		}
	}
}

boost::tuple<bool, string> CQuoteAgent::Login( const string& brokerId, const string& userId, const string& password )
{
	return false;
}

void CQuoteAgent::Logout()
{

}

bool CQuoteAgent::SubscribesQuotes( vector<string>& subscribeArr )
{
	return false;
}

bool CQuoteAgent::UnSubscribesQuotes( vector<string>& unSubscribeArr )
{
	return false;
}

void CQuoteAgent::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	logger.Info("Market connected");
	m_condConnectDone.notify_all();
}

void CQuoteAgent::OnFrontDisconnected( int nReason )
{
	boost::lock_guard<boost::mutex> lock(m_mutex);

	if(!m_bIsConnected)
		return;	// disconnect already time out 

	if(nReason == 0)
	{
		logger.Info("Market normaly disconnected.");
	}
	else
	{
		string reasonTxt = "Disconnected with market due to ";
		switch (nReason)
		{
		case 0x1001:
			reasonTxt.append("Cannot read from network");
			break;
		case 0x1002:
			reasonTxt.append("Cannot write to network");
			break;
		case 0x2001:
			reasonTxt.append("Receiving heart beat time out");
			break;
		case 0x2002:
			reasonTxt.append("Sending heart beat time out");
			break;
		case 0x2003:
			reasonTxt.append("Invalid packets received");
			break;
		}
		reasonTxt.append(" (will reconnect automatically).");
		logger.Warning(reasonTxt);
	}

	m_bIsConnected = false;
	m_condConnectDone.notify_all();
}

void CQuoteAgent::OnHeartBeatWarning( int nTimeLapse )
{
	logger.Warning(boost::str(boost::format("Hear beat warning - %d") % nTimeLapse));
}

void CQuoteAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CQuoteAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CQuoteAgent::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CQuoteAgent::OnRspSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CQuoteAgent::OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CQuoteAgent::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField *pDepthMarketData )
{

}
