#include "StdAfx.h"
#include "MarketAgent.h"
#include "LogManager.h"
#include "Configuration.h"
#include "protobuf_gen/quote.pb.h"

#include <sstream>
#include <algorithm>
#include <boost/smart_ptr.hpp>

#define SYMBOL_MAX_LENGTH 10
#define CONNECT_TIMEOUT_MINUTE 1

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
}

void RunMarketDataFunc(CThostFtdcMdApi* pUserApi, const char* address)
{
	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(strdup(address));

	pUserApi->RegisterFront(front_addr.get());					// connect
	pUserApi->Init();

	pUserApi->Join();
}

bool CMarketAgent::Connect()
{
	try{
		// 初始化UserApi
		m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
		m_pUserApi->RegisterSpi(this);						// 注册事件类

		std::stringstream ss(std::stringstream::out);
		ss << "Try to connect market (" << config.GetMarketDataAddr() << ") ...";
		logger.Info(ss.str());

		m_thQuoting = boost::thread(&RunMarketDataFunc, m_pUserApi, config.GetMarketDataAddr());
		
		// wait 1 minute for connected event
		{
			boost::unique_lock<boost::mutex> lock(m_mutex);
			if(!m_condConnectDone.timed_wait(lock, boost::posix_time::minutes(CONNECT_TIMEOUT_MINUTE)))
			{
				logger.Warning("Connecting time out");
				return false;
			}
		}
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
	logger.Info("Market connected");
	m_condConnectDone.notify_all();
}

void CMarketAgent::Disconnect()
{
	if(m_pUserApi != NULL)
		m_pUserApi->Release();
}

void CMarketAgent::OnFrontDisconnected( int nReason )
{
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
	ss << "Sending login " << ((iResult == 0) ? "Succeeded" : "Failed") << " RequestID:" << m_iRequestID - 1;
	logger.Info(ss.str());
	return (iResult == 0);
}

void CMarketAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	stringstream ss(stringstream::out);
	ss << "Login Response (ID:" << nRequestID <<")" << endl;
	if(pRspInfo->ErrorID == 0)
	{
		// login succeeded
		ss << "Login succeeded." << endl;
		ss << "Trading day: " << pRspUserLogin->TradingDay << endl;
		ss << "Login Time: " << pRspUserLogin->LoginTime << endl;
		ss << "Broker ID: " << pRspUserLogin->BrokerID << endl;
		ss << "User ID: " << pRspUserLogin->UserID << endl;
		ss << "System name: " << pRspUserLogin->SystemName << endl;
		ss << "Front ID: " << pRspUserLogin->FrontID << endl;
		ss << "Session ID: " << pRspUserLogin->SessionID << endl;
		ss << "Maximum order ref: " << pRspUserLogin->MaxOrderRef << endl;
		ss << "SHFE time: " << pRspUserLogin->SHFETime << endl;
		ss << "DCE time: " << pRspUserLogin->DCETime << endl;
		ss << "CZCE time: " << pRspUserLogin->CZCETime << endl;
		ss << "FFEX time: " << pRspUserLogin->FFEXTime << endl;
	}
	else
	{
		// login failed
		ss << "Login failed due to " << pRspInfo->ErrorMsg << endl;
	}
	logger.Info(ss.str());
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
			logger.Info("Sending logout successfully");
		}
		else
		{
			logger.Error("Sending logout failed");
		}
	}
	catch(...)
	{
		logger.Error("Unknown error happent while logging out");
	}
}

void CMarketAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	stringstream ss(stringstream::out);
	ss << "Logout Response (ID:" << nRequestID <<")" << endl;
	if(pRspInfo->ErrorID == 0)
	{
		// login succeeded
		ss << "Logout succeeded." << endl;
		ss << "Broker ID: " << pUserLogout->BrokerID << endl;
		ss << "User ID: " << pUserLogout->UserID << endl;
	}
	else
	{
		// login failed
		ss << "Logout failed due to " << pRspInfo->ErrorMsg << endl;
	}
	logger.Info(ss.str());
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
		CTP::Quote* quote = new CTP::Quote();	// it will be managed by quote aggregator

		quote->set_symbol(pDepthMarketData->InstrumentID);
		quote->set_trading_day(pDepthMarketData->TradingDay);
		quote->set_exchange_id(pDepthMarketData->ExchangeID);
		quote->set_exchange_symbol_id(pDepthMarketData->ExchangeInstID);
		quote->set_last(pDepthMarketData->LastPrice);
		quote->set_prev_settlement_price(pDepthMarketData->PreSettlementPrice);
		quote->set_prev_close(pDepthMarketData->PreClosePrice);
		quote->set_open_interest(pDepthMarketData->PreOpenInterest);
		quote->set_open(pDepthMarketData->OpenPrice);
		quote->set_high(pDepthMarketData->HighestPrice);
		quote->set_low(pDepthMarketData->LowestPrice);
		quote->set_volume(pDepthMarketData->Volume);
		quote->set_turnover(pDepthMarketData->Turnover);
		quote->set_open_interest(pDepthMarketData->OpenInterest);
		quote->set_close(pDepthMarketData->ClosePrice);
		quote->set_settlement_price(pDepthMarketData->SettlementPrice);
		quote->set_upper_limit_price(pDepthMarketData->UpperLimitPrice);
		quote->set_lower_limit_price(pDepthMarketData->LowerLimitPrice);
		quote->set_prev_delta(pDepthMarketData->PreDelta);
		quote->set_curr_delta(pDepthMarketData->CurrDelta);
		quote->set_update_time(pDepthMarketData->UpdateTime);
		quote->set_update_millisec(pDepthMarketData->UpdateMillisec);
		
		quote->set_bid(pDepthMarketData->BidPrice1);
		quote->set_bid_size(pDepthMarketData->BidVolume1);
		quote->set_ask(pDepthMarketData->AskPrice1);
		quote->set_ask_size(pDepthMarketData->AskVolume1);
		quote->set_bid_2(pDepthMarketData->BidPrice2);
		quote->set_bid_size_2(pDepthMarketData->BidVolume2);
		quote->set_ask_2(pDepthMarketData->AskPrice2);
		quote->set_ask_size_2(pDepthMarketData->AskVolume2);
		quote->set_bid_3(pDepthMarketData->BidPrice3);
		quote->set_bid_size_3(pDepthMarketData->BidVolume3);
		quote->set_ask_3(pDepthMarketData->AskPrice3);
		quote->set_ask_size_3(pDepthMarketData->AskVolume3);
		quote->set_bid_4(pDepthMarketData->BidPrice4);
		quote->set_bid_size_4(pDepthMarketData->BidVolume4);
		quote->set_ask_4(pDepthMarketData->AskPrice4);
		quote->set_ask_size_4(pDepthMarketData->AskVolume4);
		quote->set_bid_5(pDepthMarketData->BidPrice5);
		quote->set_bid_size_5(pDepthMarketData->BidVolume5);
		quote->set_ask_5(pDepthMarketData->AskPrice5);
		quote->set_ask_size_5(pDepthMarketData->AskVolume5);
		quote->set_average_price(pDepthMarketData->AveragePrice);

		m_pCallback->OnQuoteReceived(quote);
	}
}
