#include "StdAfx.h"
#include "TradeAgent.h"
#include "FileOperations.h"
#include "globalmembers.h"
#include "charsetconvert.h"

#include <sstream>

#if defined(WIN32)
#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")
#else
#define strcpy_s strcpy
#define _strdup strdup
#endif

enum { CONNECT_TIMEOUT_SECONDS = 3 };

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
	{
		logger.Debug(boost::str(boost::format("--->>> ErrorID=%d, ErrorMsg=%s")
			% pRspInfo->ErrorID % pRspInfo->ErrorMsg)); 
	}
	return bResult;
}

CTradeAgent::CTradeAgent(void)
	: m_pUserApi(NULL)
	, m_isConnected(false)
	, m_isLogged(false)
	, m_isConfirmed(false)
	, m_isWorking(false)
	, FRONT_ID(0)
	, SESSION_ID(0)
	, m_maxOrderRef(0)
	, m_iRequestID(0)
{
}


CTradeAgent::~CTradeAgent(void)
{
	Logout();
	m_pUserApi = NULL;
}

void CTradeAgent::RunTradingFunc(string address)
{
	// duplicate address string and use boost.smart_ptr to manage its life cycle
	boost::scoped_array<char> front_addr(_strdup(address.c_str()));
	m_isWorking = true;
	m_pUserApi->RegisterFront(front_addr.get());					// connect
	m_pUserApi->Init();

	m_pUserApi->Join();
	m_isWorking = false;
}

boost::tuple<bool, string> CTradeAgent::Login( const string& frontAddr, const string& brokerId, const string& userId, const string& password )
{
	try{
		string streamFolder = userId + "/Td/";
		if(!CreateFolderIfNotExists(streamFolder))
		{
			m_loginErr = boost::str(boost::format("Cannot create stream folder (%s) for trading") % streamFolder);
			return boost::make_tuple(false, m_loginErr);
		}

		m_brokerId = brokerId;
		m_investorId = userId;
		m_password = password;

		// 创建UserApi
		m_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(streamFolder.c_str());
		// 注册事件类
		m_pUserApi->RegisterSpi(this);

		m_pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);
		m_pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);

		logger.Info(boost::str(boost::format("Try to connect trade server (%s) ...") % frontAddr));

		// wait for connected event
		{
			boost::unique_lock<boost::mutex> lock(m_mutConnecting);
			m_thTrading = boost::thread(&CTradeAgent::RunTradingFunc, this, frontAddr);

			if(!m_condConnecting.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("Connecting %s time out") % frontAddr);
				logger.Warning(m_loginErr);
				return boost::make_tuple(false, m_loginErr);
			}
			if(!m_isConnected)
			{
				logger.Error(boost::str(boost::format("Connecting %s encounter unknown error") % frontAddr));
				return boost::make_tuple(false, m_loginErr);
			}
		}
		
		// wait for login
		{
			boost::unique_lock<boost::mutex> lock(m_mutLogin);
			Login();

			if(!m_condLogin.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("Login %s time out") % userId);
				logger.Warning(m_loginErr);
				return boost::make_tuple(false, m_loginErr);
			}
			if(!m_isLogged)
			{
				return boost::make_tuple(false, m_loginErr);
			}
		}

		// wait for confirm
		{
			boost::unique_lock<boost::mutex> lock(m_mutConfirm);
			ReqSettlementInfoConfirm();

			if(!m_condConfirm.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("%s Confirm time out") % userId);
				logger.Warning(m_loginErr);
				return boost::make_tuple(false, m_loginErr);
			}
			if(!m_isConfirmed)
			{
				return boost::make_tuple(false, m_loginErr);
			}
		}

		return boost::make_tuple(true, m_loginErr);
	}
	catch(std::exception& ex)
	{
		m_loginErr = "Failed to connect to trade server, details is following\n";
		m_loginErr += ex.what();
		logger.Error(m_loginErr);
	}
	catch(...)
	{
		m_loginErr = "Unknown error encountered while connecting trade server";
		logger.Error(m_loginErr);
	}

	return boost::make_tuple(false, m_loginErr);
}

void CTradeAgent::Login()
{
	string traceInfo = boost::str(boost::format("Log in trade (%s, %s, %s)") 
		% m_brokerId % m_investorId % m_password);
	logger.Trace(traceInfo);

	bool reqSucc = false;
	try
	{
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, m_brokerId.c_str());
		strcpy_s(req.UserID, m_investorId.c_str());
		strcpy_s(req.Password, m_password.c_str());
		if(m_pUserApi != NULL)
		{
			int requestId = RequestIDIncrement();
			int iResult = m_pUserApi->ReqUserLogin(&req, requestId);
			reqSucc = iResult == 0;
			string loginInfo = boost::str(boost::format("Sending login %s, RequestID: %d")
				% (reqSucc ? "Succeeded" : "Failed") % requestId);
			logger.Info(loginInfo);
		}
	}
	catch (...)
	{
		m_loginErr = "Encounter error while logging in trade server";
		logger.Error(m_loginErr);
	}

	if(!reqSucc)
	{
		boost::lock_guard<boost::mutex> lock(m_mutConnecting);
		m_isConnected = false;
		m_condConnecting.notify_one();
	}
}

void CTradeAgent::Logout()
{
	LOG_INFO(logger, boost::str(boost::format("%s Begin logging OUT...") % m_investorId));
	if(m_isWorking)
	{
		m_pUserApi->Release();
		m_thTrading.join();
	}
	LOG_INFO(logger, boost::str(boost::format("%s Logged OUT.") % m_investorId));
}

void CTradeAgent::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutConnecting);
	m_isConnected = true;
	logger.Info("Trade connected");
	m_condConnecting.notify_one();
}

void CTradeAgent::OnFrontDisconnected( int nReason )
{
	if(nReason == 0)
	{
		logger.Info(boost::str(boost::format("%s Trade normally disconnected.") % m_investorId));
	}
	else
	{
		string reasonTxt = boost::str(boost::format("%s disconnected from trade server due to ") % m_investorId);
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

	m_isConnected = false;
}

void CTradeAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	boost::mutex::scoped_lock lock(m_mutLogin);

	m_isLogged = (pRspInfo->ErrorID == 0);
	string loginInfo = boost::str(
		boost::format(
		"Trade login response (ReqId:%d): %s") 
		% nRequestID 
		% (m_isLogged ? "Succeeded" : "Failed"));
	logger.Info(loginInfo);

	if(m_isLogged)
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;

		m_maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);

		string ds(pRspUserLogin->TradingDay);
		m_tradingDay = boost::gregorian::from_undelimited_string(ds);

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
		logger.Error(boost::str(boost::format("Error Message:%s") % pRspInfo->ErrorMsg));
		GB2312ToUTF_8(m_loginErr, pRspInfo->ErrorMsg);
	}

	m_condLogin.notify_one();
}

void CTradeAgent::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	strcpy_s(req.InvestorID, m_investorId.c_str());
	int iResult = m_pUserApi->ReqSettlementInfoConfirm(&req, RequestIDIncrement());

	ostringstream debugSS;
	debugSS << "--->>> [" << m_investorId << "] 请求投资者结算结果确认: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败");
	logger.Info(debugSS.str());
}

void CTradeAgent::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if (bIsLast)
	{
		boost::unique_lock<boost::mutex> lock(m_mutConfirm);

		bool loginSuccess = false;
		string errorMsg;
		int initOrderRef = -1;

		if(!IsErrorRspInfo(pRspInfo))
		{
			// Settlement confirm succeeded, then notify login success
			loginSuccess = true;
			initOrderRef = m_maxOrderRef;
		}
		else
		{
			loginSuccess = false;
			errorMsg = pRspInfo->ErrorMsg;
			initOrderRef = -1;
		}

		m_isConfirmed = loginSuccess;
		m_loginErr = errorMsg;
		m_condConfirm.notify_one();

		//m_orderProcessor->OnRspUserLogin(loginSuccess, errorMsg, initOrderRef);
	}
}

void CTradeAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}



