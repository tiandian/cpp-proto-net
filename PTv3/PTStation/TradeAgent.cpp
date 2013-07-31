#include "StdAfx.h"
#include "TradeAgent.h"
#include "FileOperations.h"
#include "globalmembers.h"
#include "charsetconvert.h"
#include "SymbolInfoRepositry.h"
#include "RtnOrderWrapper.h"

#include <sstream>

#if defined(WIN32)
#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")
#else
#define strcpy_s strcpy
#define _strdup strdup
#endif

enum {
	DISCONNECT_TIMEOUT_SECOND = 1,
	CONNECT_TIMEOUT_SECONDS = 3,
	QUERY_QUOTE_RETRY_TIMES = 5
};

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

void PrintRtnOrder(CThostFtdcOrderField* pOrder)
{
	ostringstream oss;
	oss << "\n///////////////////////\n";
	oss << "合约代码      \t" << pOrder->InstrumentID << "\n";
	oss << "报单引用      \t" << pOrder->OrderRef << "\n";
	oss << "报单价格条件   \t" << pOrder->OrderPriceType << "\n";	
	oss << "买卖方向      \t" << pOrder->Direction << "\n";	
	oss << "组合开平标志   \t" << pOrder->CombOffsetFlag << "\n";
	oss << "组合投机套保标志\t" << pOrder->CombHedgeFlag << "\n";
	oss << "价格          \t" << pOrder->LimitPrice << "\n";
	oss << "数量          \t" << pOrder->VolumeTotalOriginal << "\n";
	oss << "有效期类型     \t" << pOrder->TimeCondition << "\n";
	oss << "成交量类型     \t" << pOrder->VolumeCondition << "\n";
	oss << "请求编号       \t" << pOrder->RequestID << "\n";
	oss << "///////////////////////\n";

	logger.Debug(oss.str());
}

CSymbolInfoRepositry symbolInfoRepo;

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
	, m_orderProcessor(NULL)
{
#ifdef FAKE_DEAL
	m_fakeDealer.Init(boost::bind(&CTradeAgent::OnRtnOrder, this, _1)
		, boost::bind(&CTradeAgent::OnRspOrderInsert, this, _1, _2, _3, _4)
		, boost::bind(&CTradeAgent::OnRspOrderAction, this, _1, _2, _3, _4));
#endif
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
#ifdef FAKE_DEAL
		m_fakeDealer.SetSessionParams(FRONT_ID, SESSION_ID);
#endif
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

		m_orderProcessor->OnRspUserLogin(loginSuccess, errorMsg, initOrderRef);
	}
}

void CTradeAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::SetCallbackHanlder( CTradeAgentCallback* pCallback )
{
	m_orderProcessor = pCallback;
}

bool CTradeAgent::SubmitOrder( trade::InputOrder* pInputOrder )
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy_s(req.BrokerID, pInputOrder->brokerid().c_str());
	///投资者代码
	strcpy_s(req.InvestorID, pInputOrder->investorid().c_str());
	///合约代码
	strcpy_s(req.InstrumentID, pInputOrder->instrumentid().c_str());
	///报单引用
	strcpy_s(req.OrderRef, pInputOrder->orderref().c_str());
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///报单价格条件: 限价
	req.OrderPriceType = pInputOrder->orderpricetype();
	///买卖方向: 
	req.Direction = pInputOrder->direction();
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = (pInputOrder->comboffsetflag())[0];
	///组合投机套保标志
	req.CombHedgeFlag[0] = (pInputOrder->combhedgeflag())[0];
	///价格
	req.LimitPrice = pInputOrder->limitprice();
	///数量: 1
	req.VolumeTotalOriginal = pInputOrder->volumetotaloriginal();
	///有效期类型: 当日有效
	req.TimeCondition = pInputOrder->timecondition();
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = pInputOrder->volumecondition();
	///最小成交量: 1
	req.MinVolume = pInputOrder->minvolume();
	///触发条件: 立即
	req.ContingentCondition = pInputOrder->contingentcondition();
	///止损价
	//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = pInputOrder->forceclosereason();
	///自动挂起标志: 否
	req.IsAutoSuspend = pInputOrder->isautosuspend();
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///用户强评标志: 否
	req.UserForceClose = pInputOrder->userforceclose();

	int iResult = m_pUserApi->ReqOrderInsert(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> 报单录入请求(OrdRef:" << pInputOrder->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", 成功" : ", 失败");
	logger.Debug(oss.str());
#endif

	return iResult == 0;
}

bool CTradeAgent::SubmitOrder( CThostFtdcInputOrderField& inputOrderField )
{
	int iRequestID = RequestIDIncrement();

	inputOrderField.RequestID = iRequestID;

#ifdef FAKE_DEAL
	int iResult = m_fakeDealer.ReqOrderInsert(&inputOrderField, iRequestID);
#else
	int iResult = m_pUserApi->ReqOrderInsert(&inputOrderField, iRequestID);
#endif

	return iResult == 0;
}

void CTradeAgent::OnRspOrderInsert( CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	ostringstream oss;
	oss << "--->>> " << "OnRspOrderInsert for order ( OrderRef: " << pInputOrder->OrderRef << " ) with RequestID: " << nRequestID <<  endl;
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		oss << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;

	logger.Info(oss.str());

	string ordRef = pInputOrder->OrderRef;
	string errorMsg = pRspInfo->ErrorMsg;
	m_orderProcessor->OnRspOrderInsert(false, ordRef, errorMsg);
}

void CTradeAgent::OnRtnOrder( CThostFtdcOrderField *pOrder )
{
	if(!IsMyOrder(pOrder))
		return;

	ostringstream oss;
	oss << "--->>> " << "OnRtnOrder (OrdRef:"  << pOrder->OrderRef << ") Status:" << pOrder->StatusMsg;
	logger.Info(oss.str());

	RtnOrderWrapperPtr orderWrapper(new CRtnOrderWrapper(pOrder));

	// Print order detail once rejected
	if(pOrder->OrderSubmitStatus >= THOST_FTDC_OSS_InsertRejected)
		PrintRtnOrder(pOrder);

	// Order will be actually managed by underlying evtOrder
	m_orderProcessor->OnRtnOrder(orderWrapper);
}

void CTradeAgent::OnRtnTrade( CThostFtdcTradeField *pTrade )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnTrade (OrdRef:"  << pTrade->OrderRef << ") TradeId:" << pTrade->TradeID;
	oss << " " << pTrade->Volume << " traded.";
	logger.Info(oss.str());

	boost::shared_ptr<trade::Trade> pTd(new trade::Trade);

	///经纪公司代码
	pTd->set_brokerid(pTrade->BrokerID);
	///投资者代码
	pTd->set_investorid(pTrade->InvestorID);
	///合约代码
	pTd->set_instrumentid(pTrade->InstrumentID);
	///报单引用
	pTd->set_orderref(pTrade->OrderRef);
	///用户代码
	pTd->set_userid(pTrade->UserID);
	///交易所代码
	pTd->set_exchangeid(pTrade->ExchangeID);
	///成交编号
	pTd->set_tradeid(pTrade->TradeID);
	///买卖方向
	pTd->set_direction(static_cast<trade::TradeDirectionType>(pTrade->Direction));
	///报单编号
	pTd->set_ordersysid(pTrade->OrderSysID);
	///会员代码
	pTd->set_participantid(pTrade->ParticipantID);
	///客户代码
	pTd->set_clientid(pTrade->ClientID);
	///交易角色
	if(pTrade->TradingRole < trade::ER_BROKER)
	{
		pTd->set_tradingrole(trade::ER_BROKER);
	}
	else
	{
		pTd->set_tradingrole(static_cast<trade::TradingRoleType>(pTrade->TradingRole));
	}
	///合约在交易所的代码
	pTd->set_exchangeinstid(pTrade->ExchangeInstID);
	///开平标志
	pTd->set_offsetflag(static_cast<trade::OffsetFlagType>(pTrade->OffsetFlag));
	///投机套保标志
	pTd->set_hedgeflag(static_cast<trade::HedgeFlagType>(pTrade->HedgeFlag));
	///价格
	pTd->set_price(pTrade->Price);
	///数量
	pTd->set_volume(pTrade->Volume);
	///成交时期
	pTd->set_tradedate(pTrade->TradeDate);
	///成交时间
	pTd->set_tradetime(pTrade->TradeTime);
	///成交类型
	if(pTrade->TradeType < trade::TRDT_COMMON)
		pTd->set_tradetype(trade::TRDT_COMMON);
	else
		pTd->set_tradetype(static_cast<trade::TradeTypeType>(pTrade->TradeType));
	///成交价来源
	if(pTrade->PriceSource < trade::PSRC_LAST_PRICE)
		pTd->set_pricesource(trade::PSRC_LAST_PRICE);
	else
		pTd->set_pricesource(static_cast<trade::PriceSourceType>(pTrade->PriceSource));
	///交易所交易员代码
	pTd->set_traderid(pTrade->TraderID);
	///本地报单编号
	pTd->set_orderlocalid(pTrade->OrderLocalID);
	///结算会员编号
	pTd->set_clearingpartid(pTrade->ClearingPartID);
	///业务单元
	pTd->set_businessunit(pTrade->BusinessUnit);
	///序号
	pTd->set_sequenceno(pTrade->SequenceNo);
	///交易日
	pTd->set_tradingday(pTrade->TradingDay);
	///结算编号
	pTd->set_settlementid(pTrade->SettlementID);
	///经纪公司报单编号
	pTd->set_brokerorderseq(pTrade->BrokerOrderSeq);

	m_orderProcessor->OnRtnTrade(pTd.get());
}

bool CTradeAgent::SubmitOrderAction( trade::InputOrderAction* pInputOrderAction )
{
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	///投资者代码
	strcpy_s(req.InvestorID, m_investorId.c_str());
	///报单操作引用
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	strcpy_s(req.OrderRef, pInputOrderAction->orderref().c_str());
	///请求编号
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///前置编号
	req.FrontID = FRONT_ID;
	///会话编号
	req.SessionID = SESSION_ID;
	///交易所代码
	strcpy_s(req.ExchangeID, pInputOrderAction->exchangeid().c_str());
	///报单编号
	strcpy_s(req.OrderSysID, pInputOrderAction->ordersysid().c_str());
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;	// Cancel order
	///价格
	//	TThostFtdcPriceType	LimitPrice;
	///数量变化
	//	TThostFtdcVolumeType	VolumeChange;
	///用户代码
	strcpy_s(req.UserID, pInputOrderAction->userid().c_str());
	///合约代码
	strcpy_s(req.InstrumentID, pInputOrderAction->instrumentid().c_str());

#ifdef FAKE_DEAL
	int iResult = m_fakeDealer.ReqOrderAction(&req, iRequestID);
#else
	int iResult = m_pUserApi->ReqOrderAction(&req, iRequestID);
#endif

	ostringstream oss;
	oss << "--->>> 报单操作请求 ( Canel OrdRef:" << pInputOrderAction->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", 成功" : ", 失败");
	logger.Debug(oss.str());

	return iResult == 0;
}

void CTradeAgent::OnRspOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	bool insertSucess = IsErrorRspInfo(pRspInfo);
	string orderRef = pInputOrderAction->OrderRef;
	string errorMsg = pRspInfo->ErrorMsg;

	m_orderProcessor->OnRspOrderAction(insertSucess, orderRef, pRspInfo->ErrorID, errorMsg);
}

void CTradeAgent::QueryAccount()
{
	assert(!m_brokerId.empty()); // , "Broker Id cannot be empty");
	assert(!m_investorId.empty());	//, "Account Id cannot be empty");

	if(m_brokerId.empty() || m_investorId.empty())
		return;

	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	strcpy_s(req.InvestorID, m_investorId.c_str());
	while (true)
	{
		int iResult = m_pUserApi->ReqQryTradingAccount(&req, RequestIDIncrement());
		if (!IsFlowControl(iResult))
		{
			std::string infoText = boost::str(boost::format("Query trading account: %d, %s") % iResult % ((iResult == 0) ? "成功" : "失败"));
			logger.Info(infoText);
			break;
		}
		else
		{
			logger.Warning(boost::str(boost::format("--->>> Query trading account: %d, 受到流控") % iResult));
			boost::this_thread::sleep(boost::posix_time::seconds(1)); 
		}
	} // while
}

void CTradeAgent::OnRspQryTradingAccount( CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	logger.Debug("--->>> OnRspQryTradingAccount");
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		boost::shared_ptr<trade::AccountInfo> account(new trade::AccountInfo);
		account->set_brokerid(pTradingAccount->BrokerID);
		account->set_accountid(pTradingAccount->AccountID);
		account->set_premortgage(pTradingAccount->PreMortgage);
		account->set_precredit(pTradingAccount->PreCredit);
		account->set_predeposit(pTradingAccount->PreDeposit);
		account->set_prebalance(pTradingAccount->PreBalance);
		account->set_premargin(pTradingAccount->PreMargin);
		account->set_interestbase(pTradingAccount->InterestBase);
		account->set_interest(pTradingAccount->Interest);
		account->set_deposit(pTradingAccount->Deposit);
		account->set_withdraw(pTradingAccount->Withdraw);
		account->set_frozenmargin(pTradingAccount->FrozenMargin);
		account->set_frozencash(pTradingAccount->FrozenCash);
		account->set_frozencommission(pTradingAccount->FrozenCommission);
		account->set_currmargin(pTradingAccount->CurrMargin);
		account->set_cashin(pTradingAccount->CashIn);
		account->set_commission(pTradingAccount->Commission);
		account->set_closeprofit(pTradingAccount->CloseProfit);
		account->set_positionprofit(pTradingAccount->PositionProfit);
		account->set_balance(pTradingAccount->Balance);
		account->set_available(pTradingAccount->Available);
		account->set_withdrawquota(pTradingAccount->WithdrawQuota);
		account->set_reserve(pTradingAccount->Reserve);
		account->set_tradingday(pTradingAccount->TradingDay);
		account->set_settlementid(pTradingAccount->SettlementID);
		account->set_credit(pTradingAccount->Credit);
		account->set_mortgage(pTradingAccount->Mortgage);
		account->set_exchangemargin(pTradingAccount->ExchangeMargin);
		account->set_deliverymargin(pTradingAccount->DeliveryMargin);
		account->set_exchangedeliverymargin(pTradingAccount->ExchangeDeliveryMargin);

		m_orderProcessor->OnRspQryTradingAccount(*account);
	}
}

void CTradeAgent::QueryOrders( const string& symbol )
{
	// to do
}

void CTradeAgent::QueryPositions()
{
	// to do
}

void CTradeAgent::QueryPositionDetails( const string& symbol )
{
	CThostFtdcQryInvestorPositionDetailField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	strcpy_s(req.InvestorID, m_investorId.c_str());
	strcpy_s(req.InstrumentID, symbol.c_str());

	while (true)
	{
		int iResult = m_pUserApi->ReqQryInvestorPositionDetail(&req, RequestIDIncrement());
		if (!IsFlowControl(iResult))
		{
			std::string infoText = boost::str(boost::format("Query investor position details: %d, %s") % iResult % ((iResult == 0) ? "成功" : "失败"));
			logger.Info(infoText);
			break;
		}
		else
		{
			logger.Warning(boost::str(boost::format("--->>> Query trading account: %d, 受到流控") % iResult));
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	} // while
}

void CTradeAgent::OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		if(pInvestorPositionDetail == NULL) return;

		boost::shared_ptr<trade::PositionDetailInfo> posiDetail(new trade::PositionDetailInfo);
		posiDetail->set_instrumentid(pInvestorPositionDetail->InstrumentID);
		posiDetail->set_brokerid(pInvestorPositionDetail->BrokerID);
		posiDetail->set_investorid(pInvestorPositionDetail->BrokerID);
		posiDetail->set_hedgeflag(static_cast<trade::HedgeFlagType>(pInvestorPositionDetail->HedgeFlag));
		posiDetail->set_direction(static_cast<trade::TradeDirectionType>(pInvestorPositionDetail->Direction));
		posiDetail->set_opendate(pInvestorPositionDetail->OpenDate);
		posiDetail->set_tradeid(pInvestorPositionDetail->TradeID);
		posiDetail->set_volume(pInvestorPositionDetail->Volume);
		posiDetail->set_openprice(pInvestorPositionDetail->OpenPrice);
		posiDetail->set_tradingday(pInvestorPositionDetail->TradingDay);
		posiDetail->set_settlementid(pInvestorPositionDetail->SettlementID);
		if(pInvestorPositionDetail->TradeType < trade::TRDT_COMMON)
			posiDetail->set_tradetype(trade::TRDT_COMMON);
		else
			posiDetail->set_tradetype(static_cast<trade::TradeTypeType>(pInvestorPositionDetail->TradeType));
		posiDetail->set_combinstrumentid(pInvestorPositionDetail->CombInstrumentID);
		posiDetail->set_exchangeid(pInvestorPositionDetail->ExchangeID);
		posiDetail->set_closeprofitbydate(pInvestorPositionDetail->CloseProfitByDate);
		posiDetail->set_closeprofitbytrade(pInvestorPositionDetail->CloseProfitByTrade);
		posiDetail->set_positionprofitbydate(pInvestorPositionDetail->PositionProfitByDate);
		posiDetail->set_positionprofitbytrade(pInvestorPositionDetail->PositionProfitByTrade);
		posiDetail->set_margin(pInvestorPositionDetail->Margin);
		posiDetail->set_exchmargin(pInvestorPositionDetail->ExchMargin);
		posiDetail->set_marginratebymoney(pInvestorPositionDetail->MarginRateByMoney);
		posiDetail->set_marginratebyvolume(pInvestorPositionDetail->MarginRateByVolume);
		posiDetail->set_lastsettlementprice(pInvestorPositionDetail->LastSettlementPrice);
		posiDetail->set_settlementprice(pInvestorPositionDetail->SettlementPrice);
		posiDetail->set_closevolume(pInvestorPositionDetail->CloseVolume);
		posiDetail->set_closeamount(pInvestorPositionDetail->CloseAmount);

		m_orderProcessor->OnRspQryInvestorPositionDetail(posiDetail.get());
	}

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> 查询仓位明细 (ReqestID: " << nRequestID << ") -- " << (bIsLast ? "End" : "Remaining");
	logger.Debug(oss.str());
#endif
}

bool CTradeAgent::QuerySymbol( const string& symbol, entity::Quote** ppQuote )
{
	bool querySucc = false;
	int retry = QUERY_QUOTE_RETRY_TIMES;
	while(retry > 0 && !querySucc)
	{
		int reqId = RequestIDIncrement();
		boost::shared_ptr< CSyncRequest<entity::Quote> > req = m_requestFactory.Create(reqId);
		bool succ = req->Invoke(boost::bind(&CTradeAgent::QuerySymbolAsync, this, symbol, _1));
		if(succ)
		{
			querySucc = req->GetResult(ppQuote);
			m_requestFactory.Remove(reqId);
			return querySucc;
		}
		else
		{
			m_requestFactory.Remove(reqId);
			boost::this_thread::sleep(boost::posix_time::milliseconds(200));
		}
		--retry;
	}
	return querySucc;
}

bool CTradeAgent::QuerySymbolAsync( const string& symbol, int nReqestId )
{
	CThostFtdcQryDepthMarketDataField req;
	strcpy_s(req.InstrumentID, symbol.c_str());
	int iResult = m_pUserApi->ReqQryDepthMarketData(&req, nReqestId);

	std::string infoText = boost::str(boost::format("Query %s Quote: %d, %s") % symbol.c_str() % iResult % ((iResult == 0) ? "成功" : "失败"));
	logger.Info(infoText);
	return (iResult == 0);
}

void CTradeAgent::OnRspQryDepthMarketData( CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
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

	m_requestFactory.Response(nRequestID, true, quote);
}

bool CTradeAgent::QuerySymbolInfo( const string& symbol, CSymbolInfo** ppSymbolInfo )
{
	bool querySucc = false;
	int reqId = RequestIDIncrement();
	boost::shared_ptr< CSyncRequest<CSymbolInfo> > req = m_symbInfoReqFactory.Create(reqId);

	CSymbolInfo* pSymbolInfo = symbolInfoRepo.Get(symbol.c_str());
	assert(pSymbolInfo != NULL);

	if(pSymbolInfo != NULL)
	{
		if(pSymbolInfo->Ready())
		{
			*ppSymbolInfo = pSymbolInfo;
			return true;
		}
		else
		{
			bool succ = req->Invoke(boost::bind(&CTradeAgent::QuerySymbolInfoAsync, this, pSymbolInfo, _1));
			if(succ)
			{
				querySucc = req->GetResult(ppSymbolInfo);
				m_requestFactory.Remove(reqId);
			}
		}
	}

	return querySucc;
}

bool CTradeAgent::QuerySymbolInfoAsync( CSymbolInfo* pSymbolInfo, int nReqestId )
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.InstrumentID, (pSymbolInfo->Instrument()).c_str());
	strcpy_s(req.ExchangeID, (pSymbolInfo->ExchangeID()).c_str());
	strcpy_s(req.ExchangeInstID, (pSymbolInfo->ExchangeInstID()).c_str());
	strcpy_s(req.ProductID, (pSymbolInfo->ProductID()).c_str());
	int iResult = m_pUserApi->ReqQryInstrument(&req, nReqestId);

	std::string infoText = boost::str(boost::format("Query %s information: %d, %s") 
		% pSymbolInfo->Instrument() % iResult % ((iResult == 0) ? "成功" : "失败"));
	logger.Info(infoText);
	return (iResult == 0);
}

void CTradeAgent::OnRspQryInstrument( CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if(pInstrument != NULL)
	{
		string symbol = pInstrument->InstrumentID;

		CSymbolInfo* pSymbInfo = symbolInfoRepo.Get(symbol.c_str());
		if(pSymbInfo != NULL)
		{
			pSymbInfo->PriceTick(pInstrument->PriceTick);
			pSymbInfo->VolumeMultiple(pInstrument->VolumeMultiple);
			pSymbInfo->Ready(true);
			m_symbInfoReqFactory.Response(nRequestID, true, pSymbInfo);
			return;
		}
	}
	// something wrong
	m_symbInfoReqFactory.Response(nRequestID, false, NULL);
}

