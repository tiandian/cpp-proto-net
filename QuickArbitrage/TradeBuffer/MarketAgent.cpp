#include "StdAfx.h"
#include "MarketAgent.h"
#include "LogManager.h"
#include "Configuration.h"

#include <sstream>
#include <algorithm>
#include <boost/smart_ptr.hpp>

#define SYMBOL_MAX_LENGTH 10

using namespace std;

extern CConfiguration config;
extern CLogManager	logger;

CMarketAgent::CMarketAgent(void):
	m_pUserApi(NULL),
	m_pCallback(NULL),
	m_iRequestID(0)
{
}

CMarketAgent::~CMarketAgent(void)
{
	if(m_pUserApi != NULL)
		m_pUserApi->Release();
}

bool CMarketAgent::Connect()
{
	try{
		// 初始化UserApi
		m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
		m_pUserApi->RegisterSpi(this);						// 注册事件类
		
		// duplicate address string and use boost.smart_ptr to manage its lifecycle
		boost::scoped_array<char> front_addr(strdup(config.GetMarketDataAddr()));
		
		m_pUserApi->RegisterFront(front_addr.get());					// connect
		m_pUserApi->Init();

		std::stringstream ss(std::stringstream::out);
		ss << "Market connected (" << front_addr << ")";
		
		logger.Info(ss.str());
		return true;
	}
	catch(std::exception& ex)
	{
		logger.Error("Failed to connect to market for quote, details is following");
		logger.Error(ex.what());
	}
	catch(...)
	{
		logger.Error("Unknown error encounted while connecting market for quote");
	}

	return false;
}

void CMarketAgent::OnFrontConnected()
{

}

void CMarketAgent::OnFrontDisconnected( int nReason )
{

}

void CMarketAgent::OnHeartBeatWarning( int nTimeLapse )
{

}

void CMarketAgent::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

bool CMarketAgent::Login( const char* brokerID, const char* userID, const char* password )
{

	std::stringstream ss(std::stringstream::out);
	ss << "Log in market (" << brokerID << " ," << userID << " ," << password << ")";
	logger.Trace(ss.str());

	int iResult = -1;

	try
	{
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, brokerID);
		strcpy(req.UserID, userID);
		strcpy(req.Password, password);
		iResult = m_pUserApi->ReqUserLogin(&req, RequestIDIncrement());
	}
	catch (...)
	{
		logger.Error("Encouter error while logging in market for quote");
	}

	ss.str("");
	ss << "Login " << ((iResult == 0) ? "Succeeded" : "Failed");
	logger.Info(ss.str());
	return (iResult == 0);
}

void CMarketAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CMarketAgent::Logout( const char* brokerID, const char* userID )
{
	logger.Trace("Logging out");

	int nResult = -1;
	try{
		CThostFtdcUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, brokerID);
		strcpy(req.UserID, userID);

		nResult = m_pUserApi->ReqUserLogout(&req, RequestIDIncrement());

		if(nResult == 0)
		{
			logger.Info("Log out successfully");
		}
		else
		{
			logger.Error("Logout failed");
		}
	}
	catch(...)
	{
		logger.Error("Unknown error happent while logging out");
	}
}

void CMarketAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

// Really subscribe symbols on market
bool CMarketAgent::SubscribesQuotes( std::vector<std::string>& subscribeArr )
{
	bool retVal = false;

	// know symbols's count
	int symbCount = subscribeArr.size();

	if(symbCount > 0)
	{
		stringstream info(std::stringstream::out);
		info <<	"Subscribe quote for symbols as following " << endl;

		// new string array
		char** symbols = new char*[symbCount];

		// loop set for copying each symbol to array
		int idx = 0;
		for (vector<string>::iterator iter = subscribeArr.begin(); 
			iter != subscribeArr.end(); ++iter, ++idx)
		{
			symbols[idx] = new char[SYMBOL_MAX_LENGTH];
			strcpy(symbols[idx], iter->c_str());
			info << iter->c_str() << " ";
		}

		try	
		{
			logger.Trace(info.str());
			// 'REALLY' submit symbols to server side
			int iResult = m_pUserApi->SubscribeMarketData(symbols, symbCount);

			if(iResult == 0)
			{
				retVal = true;
				logger.Info("Subscribe quotes successfully.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to subscribe quotes");
			}
		}
		catch(...)
		{
			logger.Error("Unknown error happent while subscribe market data for quoting");
		}

		for(int i = 0; i < symbCount; ++i)
		{
			delete[] symbols[i];
		}
		delete[] symbols; 
	}

	return retVal;
}

void CMarketAgent::OnRspSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if(m_pCallback != NULL)
	{
		m_pCallback->OnSubscribeCompleted();
	}
}

// Really unsubscribe symbols on market
bool CMarketAgent::UnSubscribesQuotes( std::vector<std::string>& unSubscribeArr )
{
	bool retVal = false;

	// know symbols's count
	int symbCount = unSubscribeArr.size();

	if(symbCount > 0)
	{
		stringstream info(std::stringstream::out);
		info <<	"Unsubscribe following symbols' quote" << endl;

		// new string array
		char** symbols = new char*[symbCount];

		// loop set for copying each symbol to array
		int idx = 0;
		for (vector<string>::iterator iter = unSubscribeArr.begin(); 
			iter !=unSubscribeArr.end(); ++iter, ++idx)
		{
			symbols[idx] = new char[SYMBOL_MAX_LENGTH];
			strcpy(symbols[idx], iter->c_str());
			info << iter->c_str() << " ";
		}

		try	
		{
			logger.Trace(info.str());
			// 'REALLY' unsubscribe symbols to server side
			int iResult = m_pUserApi->UnSubscribeMarketData(symbols, symbCount);

			if(iResult == 0)
			{
				retVal = true;
				logger.Info("Unsubscribe quotes successfully.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to unsubscribe quotes");
			}
		}
		catch(...)
		{
			logger.Error("Unknown error happent while Unsubscribe quoting");
		}

		for(int i = 0; i < symbCount; ++i)
		{
			delete[] symbols[i];
		}
		delete[] symbols; 
	}

	return retVal;
}

void CMarketAgent::OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if(m_pCallback != NULL)
	{
		m_pCallback->OnUnsubscribeCompleted();
	}
}

int CMarketAgent::RequestIDIncrement()
{
	return ++m_iRequestID;
}

void CMarketAgent::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField *pDepthMarketData )
{
	if(m_pCallback != NULL)
	{
		m_pCallback->OnQuoteReceived();
	}
}
