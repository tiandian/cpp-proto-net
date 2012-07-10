#include "StdAfx.h"
#include "TradeAgent.h"
#include "globalmembers.h"
#include "FileOperations.h"

#include <sstream>
#include <boost/format.hpp>

#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")

#define CONNECT_TIMEOUT_SECONDS 15
#define DISCONNECT_TIMEOUT_SECOND 5
#define LOGIN_TIMEOUT_SECONDS 15

// 流控判断
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

CTradeAgent::CTradeAgent(void):
m_loginSuccess(false),
m_pUserApi(NULL),
m_pCallback(NULL),
m_bIsConnected(false),
m_maxOrderRef(0)
{
}

void RunTradingFunc(CThostFtdcTraderApi* pUserApi, const char* address)
{
	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(_strdup(address));

	pUserApi->RegisterFront(front_addr.get());					// connect
	pUserApi->Init();

	pUserApi->Join();
}


CTradeAgent::~CTradeAgent(void)
{
}

boost::tuple<bool, string> CTradeAgent::Open( const string& address, const string& streamDir )
{
	string errMsg;
	try{
		string streamFolder = streamDir + "\\Td";
		if(!CreateFolderIfNotExists(streamFolder))
		{
			errMsg = boost::str(boost::format("Cannot create stream folder (%s) for trading") % streamFolder);
			return boost::make_tuple(false, errMsg);
		}
		streamFolder += "\\";
		// 初始化UserApi
		m_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(streamFolder.c_str());			// 创建UserApi
		m_pUserApi->RegisterSpi(this);						// 注册事件类

		logger.Info(boost::str(boost::format("Try to connect trade server (%s) ...") % address));

		m_thQuoting = boost::thread(&RunTradingFunc, m_pUserApi, address.c_str());

		// wait 15 seconds for connected event
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
		errMsg = "Failed to connect to trade server, details is following\n";
		errMsg = ex.what();
		logger.Error(errMsg);
	}
	catch(...)
	{
		errMsg = "Unknown error encounted while connecting trade server";
		logger.Error(errMsg);
	}

	return boost::make_tuple(false, errMsg);
}

void CTradeAgent::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	logger.Info("Trade connected");
	m_condConnectDone.notify_all();
}

void CTradeAgent::Close()
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

void CTradeAgent::OnFrontDisconnected( int nReason )
{
	boost::lock_guard<boost::mutex> lock(m_mutex);

	if(!m_bIsConnected)
		return;	// disconnect already time out 

	if(nReason == 0)
	{
		logger.Info("Trade normaly disconnected.");
	}
	else
	{
		string reasonTxt = "Disconnected from trade server due to ";
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

boost::tuple<bool, string> CTradeAgent::Login( const string& brokerId, const string& userId, const string& password )
{
	string traceInfo = boost::str(boost::format("Log in trade (%s, %s, %s)") 
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
					m_sLoginError = "Trade Login timeout";
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
		m_sLoginError = "Encouter error while logging in trade server";
		logger.Error(m_sLoginError);
	}

	return boost::make_tuple(m_loginSuccess, m_sLoginError);
}

void CTradeAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	boost::mutex::scoped_lock lock(m_mutLogin);

	m_loginSuccess = (pRspInfo->ErrorID == 0);
	string loginInfo = boost::str(
		boost::format(
		"Trade login response (ReqId:%d): %s") 
		% nRequestID 
		% (m_loginSuccess ? "Succeeded" : "Failed"));
	logger.Info(loginInfo);

	if(m_loginSuccess)
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		m_maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);

		ostringstream ss;
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

		logger.Info(ss.str());
	}
	else
	{
		m_sLoginError = pRspInfo->ErrorMsg;
		logger.Error(boost::str(boost::format("Error Message:%s") % pRspInfo->ErrorMsg));
	}
}

void CTradeAgent::Logout()
{
	logger.Trace("Trade Logging out");

	if(!m_bIsConnected)
		return;

	int nResult = -1;
	try{
		CThostFtdcUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, m_brokerID.c_str());
		strcpy_s(req.UserID, m_userID.c_str());

		if(m_pUserApi != NULL)
			nResult = m_pUserApi->ReqUserLogout(&req, RequestIDIncrement());

		if(nResult == 0)
		{
			logger.Info("Sending Trade logout successfully");
		}
		else
		{
			logger.Error("Sending Trade logout failed");
		}
	}
	catch(...)
	{
		logger.Error("Unknown error happent while Trade logging out");
	}
}

void CTradeAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	logger.Info(boost::str(boost::format("Trade Logout Response (ReqID:%d)") % nRequestID));

	string logoutInfo;
	if(pRspInfo->ErrorID == 0)
	{
		logoutInfo = boost::str(boost::format("Trade Logout succeeded. BrokerId:%s, UserId:%s") 
			% pUserLogout->BrokerID % pUserLogout->UserID);
	}
	else
	{
		// login failed
		logoutInfo = boost::str(boost::format("Trade Logout failed due to %s") % pRspInfo->ErrorMsg);
	}

	logger.Info(logoutInfo);
}

void CTradeAgent::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspQryInstrument( CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspQryTradingAccount( CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspOrderInsert( CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}


void CTradeAgent::OnHeartBeatWarning( int nTimeLapse )
{

}

int CTradeAgent::RequestIDIncrement()
{
	boost::mutex::scoped_lock lock(m_mutex);
	return ++m_iRequestID;
}
