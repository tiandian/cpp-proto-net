#include "StdAfx.h"
#include "TradeAgent.h"
#include "Configuration.h"
#include "LogManager.h"
#include "OrderManager.h"

#include <sstream>

#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")

#define LOGOUT_TIMEOUT_SECOND 5
#define WAIT_SECONDS_FOR_FLOWCONTROL 1

extern CConfiguration config;
extern CLogManager	logger;

using namespace std;

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

CTradeAgent::CTradeAgent(COrderManager* pOrderMgr):
	m_isConnected(false),
	m_pUserApi(NULL)
{
	m_pOrderMgr = pOrderMgr;
}


CTradeAgent::~CTradeAgent(void)
{
}

void TradingThreadFunc(CThostFtdcTraderApi* pUserApi, const char* address)
{
	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(strdup(address));

	pUserApi->RegisterFront(front_addr.get());					// connect
	pUserApi->Init();

	pUserApi->Join();
}

void CTradeAgent::Login( const std::string& brokerId, const std::string& userId, const std::string& password )
{
	try{
		// 初始化UserApi

		m_brokerId = brokerId;
		m_userId = userId;
		m_password = password;

		m_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\Td\\");			// 创建UserApi
		m_pUserApi->RegisterSpi(this);						// 注册事件类
		m_pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// 注册公有流
		m_pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);	

		std::ostringstream ss;
		ss << "Try to trading front (" << config.GetTradeAddr() << ") ...";
		logger.Info(ss.str());

		m_thTrading = boost::thread(&TradingThreadFunc, m_pUserApi, config.GetTradeAddr());
	}
	catch(std::exception& ex)
	{
		logger.Error("Failed to log in to trading front, details is following");
		logger.Error(ex.what());
	}
	catch(...)
	{
		logger.Error("Unknown error encounted while logging in trading front");
	}

}

void CTradeAgent::OnFrontConnected()
{
	if(m_userId.length() > 0)
	{
		logger.Debug("Trading front connected, then begin logging in");

		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, m_brokerId.c_str());
		strcpy(req.UserID, m_userId.c_str());
		strcpy(req.Password, m_password.c_str());

		int iResult = m_pUserApi->ReqUserLogin(&req, RequestIDIncrement());
		std::ostringstream info;
		info << "--->>> Send request of log in trading front (Result:" << iResult << ((iResult == 0) ? "), Succeeded" : ", Failed");
		logger.Info(info.str());
	}
	else
	{
		logger.Debug("Unexpected front connected event");
	}
}

void CTradeAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	std::string errorMsg;
	std::ostringstream ss;
	ss << "Login Response (ID:" << nRequestID <<")" << endl;
	if(pRspInfo->ErrorID == 0)
	{
		m_isConnected = true;

		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);

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

		ReqSettlementInfoConfirm();
	}
	else
	{
		// login failed
		errorMsg = pRspInfo->ErrorMsg;
		ss << "Login failed due to " << pRspInfo->ErrorMsg << endl;

		m_pOrderMgr->OnRspUserLogin(m_isConnected, errorMsg);
	}

	logger.Info(ss.str());
}

void CTradeAgent::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId.c_str());
	strcpy(req.InvestorID, m_userId.c_str());
	int iResult = m_pUserApi->ReqSettlementInfoConfirm(&req, RequestIDIncrement());

	ostringstream debugSS;
	debugSS << "--->>> 请求投资者结算结果确认: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败");
	logger.Info(debugSS.str());
}

void CTradeAgent::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	string errorMsg;
	if (bIsLast)
	{
		if(!IsErrorRspInfo(pRspInfo))
		{
			// Settlement confirm succeeded, then notify login success
			m_pOrderMgr->OnRspUserLogin(true, errorMsg);
		}
		else
		{
			errorMsg = pRspInfo->ErrorMsg;
			m_pOrderMgr->OnRspUserLogin(false, errorMsg);
		}
	}
}

void CTradeAgent::Logout()
{
	logger.Trace("Logging out...");

	if(!m_isConnected)
		return;

	int nResult = -1;
	try{
		CThostFtdcUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, m_brokerId.c_str());
		strcpy(req.UserID, m_userId.c_str());

		m_brokerId.clear();
		m_userId.clear();

		{
			boost::unique_lock<boost::mutex> lock(m_mutex);

			if(m_pUserApi != NULL)
				nResult = m_pUserApi->ReqUserLogout(&req, RequestIDIncrement());

			if(nResult == 0)
			{
				logger.Info("Sending logout from trading front successfully");
				if(!m_condLogout.timed_wait(lock, boost::posix_time::seconds(LOGOUT_TIMEOUT_SECOND)))
				{
					logger.Warning("logging out from trade front time out.");
					m_isConnected = false;
				}
			}
			else
			{
				logger.Error("Sending logout from trading front failed");
			}
		}

		m_pUserApi->Release();
		m_pUserApi = NULL;
	}
	catch(...)
	{
		logger.Error("Unknown error happent while logging out from trading front");
	}
}

void CTradeAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	boost::lock_guard<boost::mutex> lock(m_mutex);

	if(!m_isConnected)
		return;

	ostringstream ss;
	ss << "Log out from trading front. Response (ID:" << nRequestID <<")" << endl;
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
	
	m_isConnected = false;
	m_condLogout.notify_one();

	logger.Info(ss.str());
}

void CTradeAgent::OnRspQryInstrument( CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspQryTradingAccount( CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询投资者持仓
		//ReqQryInvestorPosition();
	}
}

void CTradeAgent::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspOrderInsert( CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	cerr << "--->>> " << "OnRspOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTradeAgent::OnRspOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTradeAgent::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnFrontDisconnected( int nReason )
{
	if(!m_isConnected)
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
}

void CTradeAgent::OnHeartBeatWarning( int nTimeLapse )
{

}

void CTradeAgent::OnRtnOrder( CThostFtdcOrderField *pOrder )
{
	cerr << "--->>> " << "OnRtnOrder"  << endl;
	if (IsMyOrder(pOrder))
	{
		/*if (IsTradingOrder(pOrder))
		ReqOrderAction(pOrder);
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
		cout << "--->>> 撤单成功" << endl;*/
	}
}

void CTradeAgent::OnRtnTrade( CThostFtdcTradeField *pTrade )
{
	cerr << "--->>> " << "OnRtnTrade"  << endl;
}

void CTradeAgent::Buy()
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, m_brokerId.c_str());
	///投资者代码
	strcpy(req.InvestorID, m_userId.c_str());
	///合约代码
	strcpy(req.InstrumentID, "cu1206");
	///报单引用
	strcpy(req.OrderRef, ORDER_REF);
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///买卖方向: 
	req.Direction = THOST_FTDC_D_Buy;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///价格
	req.LimitPrice = 61000;
	///数量: 1
	req.VolumeTotalOriginal = 1;
	///有效期类型: 当日有效
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	req.MinVolume = 1;
	///触发条件: 立即
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///止损价
	//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///用户强评标志: 否
	req.UserForceClose = 0;

	int iResult = m_pUserApi->ReqOrderInsert(&req, RequestIDIncrement());
	cerr << "--->>> 报单录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
}

void CTradeAgent::QueryAccount()
{
	if(m_isConnected)
	{
		logger.Warning("Didn't log in trading front!!!");
		return;
	}

	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId.c_str());
	strcpy(req.InvestorID, m_userId.c_str());
	while (true)
	{
		int iResult = m_pUserApi->ReqQryTradingAccount(&req, RequestIDIncrement());
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> 请求查询资金账户: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
			break;
		}
		else
		{
			cerr << "--->>> 请求查询资金账户: "  << iResult << ", 受到流控" << endl;
			boost::this_thread::sleep(boost::posix_time::seconds(WAIT_SECONDS_FOR_FLOWCONTROL));
		}
	} // while
}

bool CTradeAgent::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
		(pOrder->SessionID == SESSION_ID) &&
		(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CTradeAgent::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}


