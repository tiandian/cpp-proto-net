#pragma once

#include "TradeAgentCallback.h"
#include "multilegorderptr.h"
#include "MLOrderStateMachine.h"
#include "SgOrderStateMachine.h"
#include "OrderProcPushFuncDef.h"
#include "PortfolioOrderHelper.h"

#include "boost/thread.hpp"
#include <boost/tuple/tuple.hpp>

class CPortfolio;
class CTradeAgent;

class COrderProcessor2 : public CTradeAgentCallback
{
public:
	COrderProcessor2(void);
	~COrderProcessor2(void);

	void Initialize(CTradeAgent* pTradeAgent);

	void SubmitPortfOrder(CPortfolio* pPortf, const MultiLegOrderPtr& multilegOrder);
	COrderPlacer* CreateSingleOrderPlacer(CPortfolio* pPortf, trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes);
	COrderPlacer* CreateScaplerPlacer(CPortfolio* pPortf, trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes);
	void RaiseMLOrderPlacerEvent(const string& mlOrdPlacerId, COrderEvent& orderEvent);
	
	int LockForSubmit(string& outOrdRef);
	bool SubmitAndUnlock(trade::InputOrder* pOrder);
	int GenerateOrderRef(string& outOrdRef);
	bool SubmitOrderToTradeAgent(trade::InputOrder* pOrder);

	void CancelOrder(const std::string& ordRef, 
					 const std::string& exchId, 
					 const std::string& ordSysId, 
					 const std::string& userId,
					 const std::string& symbol);

	boost::tuple<bool, string> PlaceOrder( const string& symbol, 
										   trade::TradeDirectionType direction, 
										   const string& openDate, 
										   PlaceOrderContext* placeOrderCtx);
	bool QuerySymbol( const std::string& symbol, entity::Quote** ppQuote );

	bool QueryAccountInfo(string* outSerializedAcctInfo);
	void QueryPositionDetails(const string& symbol);

	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void SetPushPortfolioFunc(PushMultiLegOrderFunc funcPushMLOrder);

	void PublishOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void SetPushOrderFunc(PushLegOrderFunc funcPushOrder);

	void PublishTradeUpdate(trade::Trade* pTrade);
	void SetPushTradeFunc(PushTradeFunc funcPushTrade);

	void PublishPositionDetail(trade::PositionDetailInfo* pPosiDetailInfo);
	void SetPushPositionDetailFunc(PushPositionDetailFunc funcPushPosiDetail);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///登录请求响应
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///报单录入请求响应
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///报单操作请求响应
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg);

	///报单通知
	virtual void OnRtnOrder(trade::Order* order);

	///成交通知
	virtual void OnRtnTrade(trade::Trade* pTrade);

	///请求查询投资者持仓明细响应
	virtual void OnRspQryInvestorPositionDetail(trade::PositionDetailInfo* pPositionDetail);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	virtual void OnRspQryInstrument(){}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo);

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

	bool IsReadyForSubmit(){ return !m_bIsSubmitting; }
	int TotalOpenTimes(){ return m_totalOpenTimes; }
	int TotalCancelTimes(){ return m_totalCancelTimes; }

	bool ReachOpenTimesLimit(){ return m_totalOpenTimes >= m_maxTotalOpenTimes; }
	bool ReachCancelTimesLimit() { return m_totalCancelTimes >= m_maxTotalCancelTimes; }

	void SetMaxOpenTimes(int maxOpenTimes){ m_maxTotalOpenTimes = maxOpenTimes; }
	void SetMaxCancelTimes(int maxCancelTimes){ m_maxTotalCancelTimes = maxCancelTimes; }

private:

	static void PrintOrderStatus(trade::Order* order);
	static bool GetOrderEvent(trade::Order* order, COrderEvent** ppOrderEvt);
	
	trade::InputOrder* BuildCloseOrder(const string& symbol, trade::TradeDirectionType direction, const string& openDate, PlaceOrderContext* placeOrderCtx);
	void AddOpenTimes() { ++m_totalOpenTimes; }
	void AddCancelTimes() { ++m_totalCancelTimes; }

	CMLOrderStateMachine m_mlOrderStateMachine;
	CSgOrderStateMachine m_sgOrderStateMachine;

	int m_maxOrderRef;
	boost::mutex m_mutOrdRefIncr;
	boost::condition_variable m_condSubmit;
	bool m_bIsSubmitting;

	PushMultiLegOrderFunc m_pushMultiOrdFunc;
	PushLegOrderFunc m_pushLegOrderFunc;
	PushTradeFunc m_pushTradeFunc;
	PushPositionDetailFunc m_pushPosiDetailFunc;

	CTradeAgent* m_pTradeAgent;

	boost::condition_variable m_condQryAcct;
	boost::mutex m_mutQryAcct;
	string m_serializedQryAcctInfo;

	int m_totalOpenTimes;
	int m_totalCancelTimes;
	int m_maxTotalOpenTimes;
	int m_maxTotalCancelTimes;
};

