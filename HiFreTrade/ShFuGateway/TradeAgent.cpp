#include "StdAfx.h"
#include "TradeAgent.h"
#include "LogManager.h"
#include "ReturnOrder.h"
#include "Trade.h"

#include <sstream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")

#define LOGOUT_TIMEOUT_SECOND 5
#define WAIT_SECONDS_FOR_FLOWCONTROL 1

const char* TradeAddress = "tcp://asp-sim2-front1.financial-trading-platform.com:26205";

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

CTradeAgent::CTradeAgent(void):
	m_isConnected(false),
	m_maxOrderRef(-1),
	m_pUserApi(NULL)
{
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
		ss << "Try to trading front (" << TradeAddress << ") ...";
		logger.Info(ss.str());

		m_thTrading = boost::thread(&TradingThreadFunc, m_pUserApi, TradeAddress);
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
		m_maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		//sprintf(ORDER_REF, "%d", iNextOrderRef);

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
	if (bIsLast)
	{
		if(!IsErrorRspInfo(pRspInfo))
		{
			// Settlement confirm succeeded, then notify login success
			logger.Info(boost::str(boost::format("SettlementInfo Confirm Succeeded. MaxOrderRef:%d") % m_maxOrderRef));
		}
		else
		{
			string errorMsg = pRspInfo->ErrorMsg;
			logger.Info(boost::str(boost::format("SettlementInfo Confirm Failed. Error:%s") % errorMsg));
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
		
	}
}

void CTradeAgent::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

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

bool CTradeAgent::SubmitOrder( const std::vector< boost::shared_ptr<CInputOrder> >& orders )
{
	bool suc = true;

	BOOST_FOREACH(const boost::shared_ptr<CInputOrder>& ord, orders)
	{
		if(!SubmitOrder(ord.get()))
			suc = false;
	}

	return suc;
}

bool CTradeAgent::SubmitOrder( CInputOrder* pOrder )
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, m_brokerId.c_str());
	///投资者代码
	strcpy(req.InvestorID, m_userId.c_str());
	///合约代码
	strcpy(req.InstrumentID, pOrder->instrumentid().c_str());
	///报单引用
	strcpy(req.OrderRef, pOrder->orderref().c_str());
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///报单价格条件: 限价
	req.OrderPriceType = pOrder->orderpricetype();
	///买卖方向: 
	req.Direction = pOrder->direction();
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = (pOrder->comboffsetflag())[0];
	///组合投机套保标志
	req.CombHedgeFlag[0] = (pOrder->combhedgeflag())[0];
	///价格
	req.LimitPrice = pOrder->limitprice();
	///数量: 1
	req.VolumeTotalOriginal = pOrder->volumetotaloriginal();
	///有效期类型: 当日有效
	req.TimeCondition = pOrder->timecondition();
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = pOrder->volumecondition();
	///最小成交量: 1
	req.MinVolume = pOrder->minvolume();
	///触发条件: 立即
	req.ContingentCondition = pOrder->contingentcondition();
	///止损价
	//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = pOrder->forceclosereason();
	///自动挂起标志: 否
	req.IsAutoSuspend = pOrder->isautosuspend();
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///用户强评标志: 否
	req.UserForceClose = pOrder->userforceclose();

	int iResult = m_pUserApi->ReqOrderInsert(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> 报单录入请求(OrdRef:" << pOrder->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", 成功" : ", 失败");
	logger.Debug(oss.str());
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
}

void CTradeAgent::OnRtnOrder( CThostFtdcOrderField *pOrder )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnOrder (OrdRef:"  << pOrder->OrderRef << ") Status:" << pOrder->StatusMsg;
	logger.Info(oss.str());

	CReturnOrder* pOrd = new CReturnOrder;

	///经纪公司代码
	pOrd->set_brokerid(pOrder->BrokerID);
	///投资者代码
	pOrd->set_investorid(pOrder->InvestorID);
	///合约代码
	pOrd->set_instrumentid(pOrder->InstrumentID);
	///报单引用
	pOrd->set_orderref(pOrder->OrderRef);
	///用户代码
	pOrd->set_userid(pOrder->UserID);
	///报单价格条件
	pOrd->set_orderpricetype(static_cast	<OrderPriceTypeType>(pOrder->OrderPriceType));
	///买卖方向
	pOrd->set_direction(static_cast<TradeDirectionType>(pOrder->Direction));
	///组合开平标志
	pOrd->set_comboffsetflag(pOrder->CombOffsetFlag);
	///组合投机套保标志
	pOrd->set_combhedgeflag(pOrder->CombHedgeFlag);
	///价格
	pOrd->set_limitprice(pOrder->LimitPrice);
	///数量
	pOrd->set_volumetotaloriginal(pOrder->VolumeTotalOriginal);
	///有效期类型
	pOrd->set_timecondition(static_cast<TimeConditionType>(pOrder->TimeCondition));
	///GTD日期
	pOrd->set_gtddate(pOrder->GTDDate);
	///成交量类型
	pOrd->set_volumecondition(static_cast<VolumeConditionType>(pOrder->VolumeCondition));
	///最小成交量
	pOrd->set_minvolume(pOrder->MinVolume);
	///触发条件
	pOrd->set_contingentcondition(static_cast<:ContingentConditionType>(pOrder->ContingentCondition));
	///止损价
	pOrd->set_stopprice(pOrder->StopPrice);
	///强平原因
	pOrd->set_forceclosereason(static_cast<ForceCloseReasonType>(pOrder->ForceCloseReason));
	///自动挂起标志
	pOrd->set_isautosuspend(pOrder->IsAutoSuspend);
	///业务单元
	pOrd->set_businessunit(pOrder->BusinessUnit);
	///请求编号
	pOrd->set_requestid(pOrder->RequestID);
	// Above fields are same as InputOrder's
	////////////////////////////////////////////////////////////////////

	///本地报单编号
	pOrd->set_orderlocalid(pOrder->OrderLocalID);
	///交易所代码
	pOrd->set_exchangeid(pOrder->ExchangeID);
	///会员代码
	pOrd->set_participantid(pOrder->ParticipantID);
	///客户代码
	pOrd->set_clientid(pOrder->ClientID);
	///合约在交易所的代码
	pOrd->set_exchangeinstid(pOrder->ExchangeInstID);
	///交易所交易员代码
	pOrd->set_traderid(pOrder->TraderID);
	///安装编号
	pOrd->set_installid(pOrder->InstallID);
	///报单提交状态
	pOrd->set_ordersubmitstatus(static_cast<OrderSubmitStatusType>(pOrder->OrderSubmitStatus));
	///报单提示序号
	pOrd->set_notifysequence(pOrder->NotifySequence);
	///交易日
	pOrd->set_tradingday(pOrder->TradingDay);
	///结算编号
	pOrd->set_settlementid(pOrder->SettlementID);
	///报单编号
	pOrd->set_ordersysid(pOrder->OrderSysID);
	///报单来源
	// sometimes OrderSource could be 0 insteade of '0'
	if(pOrder->OrderSource < PARTICIPANT)
		pOrd->set_ordersource(PARTICIPANT);
	else
		pOrd->set_ordersource(static_cast<OrderSourceType>(pOrder->OrderSource));
	///报单状态
	pOrd->set_orderstatus(static_cast<OrderStatusType>(pOrder->OrderStatus));
	///报单类型
	// Sometimes OrderType could be 0 instead of '0'
	if(pOrder->OrderType < NORMAL_ORDER)
		pOrd->set_ordertype(NORMAL_ORDER);
	else
		pOrd->set_ordertype(static_cast<OrderTypeType>(pOrder->OrderType));
	///今成交数量
	pOrd->set_volumetraded(pOrder->VolumeTraded);
	///剩余数量
	pOrd->set_volumetotal(pOrder->VolumeTotal);
	///报单日期
	pOrd->set_insertdate(pOrder->InsertDate);
	///委托时间
	pOrd->set_inserttime(pOrder->InsertTime);
	///激活时间
	pOrd->set_activetime(pOrder->ActiveTime);
	///挂起时间
	pOrd->set_suspendtime(pOrder->SuspendTime);
	///最后修改时间
	pOrd->set_updatetime(pOrder->UpdateTime);
	///撤销时间
	pOrd->set_canceltime(pOrder->CancelTime);
	///最后修改交易所交易员代码
	pOrd->set_activetraderid(pOrder->ActiveTraderID);
	///结算会员编号
	pOrd->set_clearingpartid(pOrder->ClearingPartID);
	///序号
	pOrd->set_sequenceno(pOrder->SequenceNo);
	///前置编号
	pOrd->set_frontid(pOrder->FrontID);
	///会话编号
	pOrd->set_sessionid(pOrder->SessionID);
	///用户端产品信息
	pOrd->set_userproductinfo(pOrder->UserProductInfo);
	///状态信息
	pOrd->set_statusmsg(pOrder->StatusMsg);
	///用户强评标志
	pOrd->set_userforceclose(pOrder->UserForceClose);
	///操作用户代码
	pOrd->set_activeuserid(pOrder->ActiveUserID);
	///经纪公司报单编号
	pOrd->set_brokerorderseq(pOrder->BrokerOrderSeq);
	///相关报单
	pOrd->set_relativeordersysid(pOrder->RelativeOrderSysID);
}

void CTradeAgent::OnRtnTrade( CThostFtdcTradeField *pTrade )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnTrade (OrdRef:"  << pTrade->OrderRef << ") TradeId:" << pTrade->TradeID;
	logger.Info(oss.str());

	CTrade* pTd = new CTrade;

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
	pTd->set_direction(static_cast<TradeDirectionType>(pTrade->Direction));
	///报单编号
	pTd->set_ordersysid(pTrade->OrderSysID);
	///会员代码
	pTd->set_participantid(pTrade->ParticipantID);
	///客户代码
	pTd->set_clientid(pTrade->ClientID);
	///交易角色
	if(pTrade->TradingRole < ER_BROKER)
	{
		pTd->set_tradingrole(ER_BROKER);
	}
	else
	{
		pTd->set_tradingrole(static_cast<TradingRoleType>(pTrade->TradingRole));
	}
	///合约在交易所的代码
	pTd->set_exchangeinstid(pTrade->ExchangeInstID);
	///开平标志
	pTd->set_offsetflag(static_cast<OffsetFlagType>(pTrade->OffsetFlag));
	///投机套保标志
	pTd->set_hedgeflag(static_cast<HedgeFlagType>(pTrade->HedgeFlag));
	///价格
	pTd->set_price(pTrade->Price);
	///数量
	pTd->set_volume(pTrade->Volume);
	///成交时期
	pTd->set_tradedate(pTrade->TradeDate);
	///成交时间
	pTd->set_tradetime(pTrade->TradeTime);
	///成交类型
	if(pTrade->TradeType < TRDT_COMMON)
		pTd->set_tradetype(TRDT_COMMON);
	else
		pTd->set_tradetype(static_cast<TradeTypeType>(pTrade->TradeType));
	///成交价来源
	if(pTrade->PriceSource < PSRC_LAST_PRICE)
		pTd->set_pricesource(PSRC_LAST_PRICE);
	else
		pTd->set_pricesource(static_cast<PriceSourceType>(pTrade->PriceSource));
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

	//m_pOrderMgr->OnRtnTrade(pTd);
}

bool CTradeAgent::SubmitOrderAction( CInputOrderAction* pOrderAction )
{
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, m_brokerId.c_str());
	///投资者代码
	strcpy(req.InvestorID, m_userId.c_str());
	///报单操作引用
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	strcpy(req.OrderRef, pOrderAction->orderref().c_str());
	///请求编号
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///前置编号
	req.FrontID = FRONT_ID;
	///会话编号
	req.SessionID = SESSION_ID;
	///交易所代码
	strcpy(req.ExchangeID, pOrderAction->exchangeid().c_str());
	///报单编号
	strcpy(req.OrderSysID, pOrderAction->ordersysid().c_str());
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;	// Cancel order
	///价格
	//	TThostFtdcPriceType	LimitPrice;
	///数量变化
	//	TThostFtdcVolumeType	VolumeChange;
	///用户代码
	strcpy(req.UserID, pOrderAction->userid().c_str());
	///合约代码
	strcpy(req.InstrumentID, pOrderAction->instrumentid().c_str());

	int iResult = m_pUserApi->ReqOrderAction(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> 报单操作请求 ( Canel OrdRef:" << pOrderAction->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", 成功" : ", 失败");
	logger.Debug(oss.str());
#endif

	return iResult == 0;
}

void CTradeAgent::OnRspOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	//m_pOrderMgr->OnRspOrderInsert(IsErrorRspInfo(pRspInfo), std::string(pInputOrderAction->OrderRef), std::string(pRspInfo->ErrorMsg));
}
