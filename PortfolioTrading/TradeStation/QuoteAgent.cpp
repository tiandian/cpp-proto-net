#include "StdAfx.h"
#include "QuoteAgent.h"
#include "globalmembers.h"
#include "FileOperations.h"

#include <boost/format.hpp>
#include <sstream>

#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")

#define SYMBOL_MAX_LENGTH 10
#define CONNECT_TIMEOUT_SECONDS 15
#define DISCONNECT_TIMEOUT_SECOND 5
#define LOGIN_TIMEOUT_SECONDS 15

CQuoteAgent::CQuoteAgent(void):
m_loginSuccess(false),
m_pUserApi(NULL),
m_pCallback(NULL),
m_bIsConnected(false),
m_iRequestID(0)
{
}


CQuoteAgent::~CQuoteAgent(void)
{
}

void RunMarketDataFunc(CThostFtdcMdApi* pUserApi, const char* address)
{
	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(_strdup(address));

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
	string traceInfo = boost::str(boost::format("Log in quote (%s, %s, %s)") 
		% brokerId.c_str() % userId % password);
	logger.Trace(traceInfo);
	
	m_brokerID = brokerId;
	m_userID = userId;
	
	try
	{
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, brokerId.c_str());
		strcpy_s(req.UserID, userId.c_str());
		strcpy_s(req.Password, password.c_str());
		if(m_pUserApi != NULL)
		{
			boost::unique_lock<boost::mutex> lock(m_mutLogin);

			int requestId = RequestIDIncrement();
			int iResult = m_pUserApi->ReqUserLogin(&req, requestId);
			bool reqSucc = iResult == 0;
			string loginInfo = boost::str(boost::format("Sending login %s, RequestID: %d")
				% (reqSucc ? "Succeeded" : "Failed") % requestId);
			logger.Info(loginInfo);
			
			if(reqSucc)
			{
				if(!m_condLogin.timed_wait(lock, boost::posix_time::seconds(LOGIN_TIMEOUT_SECONDS)))
				{
					m_sLoginError = "Login timeout";
					logger.Error(m_sLoginError);
				}
			}
			else
			{
				m_sLoginError = loginInfo;
			}
		}
	}
	catch (...)
	{
		m_sLoginError = "Encouter error while logging in market for quote";
		logger.Error(m_sLoginError);
	}

	return boost::make_tuple(m_loginSuccess, m_sLoginError);
}

void CQuoteAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	boost::mutex::scoped_lock lock(m_mutLogin);

	m_loginSuccess = (pRspInfo->ErrorID == 0);
	m_condLogin.notify_one();
	string loginInfo = boost::str(
		boost::format(
		"Quote login response (ReqId:%d): %s") 
		% nRequestID 
		% (m_loginSuccess ? "Succeeded" : "Failed"));
	logger.Info(loginInfo);

	if(!m_loginSuccess)
	{
		m_sLoginError = pRspInfo->ErrorMsg;
		logger.Error(boost::str(boost::format("Error Message:%s") % pRspInfo->ErrorMsg));
	}
}

void CQuoteAgent::Logout()
{
	logger.Trace("Logging out");

	if(!m_bIsConnected || !m_loginSuccess)
		return;

	int nResult = -1;
	try{
		CThostFtdcUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, m_brokerID.c_str());
		strcpy_s(req.UserID, m_userID.c_str());

		if(m_pUserApi != NULL)
		{
			int reqId = RequestIDIncrement();
			nResult = m_pUserApi->ReqUserLogout(&req, reqId);
		}

		if(nResult == 0)
		{
			logger.Info("Sending logout successfully");
			m_loginSuccess = false;
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

void CQuoteAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	logger.Info(boost::str(boost::format("Logout Response (ReqID:%d)") % nRequestID));

	string logoutInfo;
	if(pRspInfo->ErrorID == 0)
	{
		logoutInfo = boost::str(boost::format("Logout succeeded. BrokerId:%s, UserId:%s") 
									% pUserLogout->BrokerID % pUserLogout->UserID);
	}
	else
	{
		// login failed
		logoutInfo = boost::str(boost::format("Logout failed due to %s") % pRspInfo->ErrorMsg);
	}

	logger.Info(logoutInfo);
}

bool CQuoteAgent::SubscribesQuotes( vector<string>& subscribeArr )
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
			strcpy_s(symbols[idx], SYMBOL_MAX_LENGTH, iter->c_str());
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
				logger.Info("Subscribe quotes to server sent.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to send subscribe quotes");
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

void CQuoteAgent::OnRspSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	stringstream txt(stringstream::out); 
	txt << "Subscribe '" << pSpecificInstrument->InstrumentID;

	if(pRspInfo->ErrorID == 0)
	{
		txt << "' SUCCEEDED.";
		logger.Info(txt.str());
	}
	else
	{
		txt << "' FAILED due to ";
		txt << pRspInfo->ErrorMsg;
		logger.Warning(txt.str());
	}

	if(m_pCallback != NULL)
	{
		m_pCallback->OnSubscribeCompleted();
	}
}

void CQuoteAgent::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField *pDepthMarketData )
{
	logger.Debug(boost::str(boost::format("Quote (%s): %d")
		% pDepthMarketData->InstrumentID % pDepthMarketData->LastPrice));

	if(m_pCallback != NULL)
	{
		entity::Quote* quote = new entity::Quote();	// it will be managed by quote aggregator

		quote->set_symbol(pDepthMarketData->InstrumentID);
		quote->set_trading_day(pDepthMarketData->TradingDay);
		quote->set_exchange_id(pDepthMarketData->ExchangeID);
		quote->set_exchange_symbol_id(pDepthMarketData->ExchangeInstID);
		quote->set_last(pDepthMarketData->LastPrice);
		quote->set_prev_settlement_price(pDepthMarketData->PreSettlementPrice);
		quote->set_prev_close(pDepthMarketData->PreClosePrice);
		quote->set_prev_open_interest(pDepthMarketData->PreOpenInterest);
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


bool CQuoteAgent::UnSubscribesQuotes( vector<string>& unSubscribeArr )
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
			strcpy_s(symbols[idx], SYMBOL_MAX_LENGTH, iter->c_str());
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
				logger.Info("Unsubscribe quotes sent.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to send unsubscribe quotes");
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

void CQuoteAgent::OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	stringstream txt(stringstream::out); 
	txt << "UnSubscribe '" << pSpecificInstrument->InstrumentID;
	if(pRspInfo->ErrorID == 0)
	{
		txt << "' succeeded.";
		logger.Trace(txt.str());
	}
	else
	{
		txt << "' failed due to ";
		txt << pRspInfo->ErrorMsg;
		logger.Warning(txt.str());
	}

	if(m_pCallback != NULL)
	{
		m_pCallback->OnUnsubscribeCompleted();
	}
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

void CQuoteAgent::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

int CQuoteAgent::RequestIDIncrement()
{
	boost::mutex::scoped_lock lock(m_mutex);
	return m_iRequestID++;
}
