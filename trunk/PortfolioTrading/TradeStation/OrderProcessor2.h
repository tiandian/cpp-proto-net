#pragma once

#include "TradeAgentCallback.h"
#include "multilegorderptr.h"
#include "MLOrderStateMachine.h"
#include "SgOrderStateMachine.h"

#include "boost/thread.hpp"

class CPortfolio;

class COrderProcessor2 : public CTradeAgentCallback
{
public:
	COrderProcessor2(void);
	~COrderProcessor2(void);

	void SubmitPortfOrder(CPortfolio* pPortf, const MultiLegOrderPtr& multilegOrder);
	OrderPlacerPtr CreateSingleOrderPlacer(trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes);
	void RaiseMLOrderPlacerEvent(const string& mlOrdPlacerId, COrderEvent& orderEvent);

	int GenerateOrderRef(char** ppOrdRef);

	void CancelOrder(const std::string& ordRef, 
					 const std::string& exchId, 
					 const std::string& ordSysId, 
					 const std::string& userId,
					 const std::string& symbol);

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

private:

	static void PrintOrderStatus(trade::Order* order);
	static bool GetOrderEvent(trade::Order* order, COrderEvent** ppOrderEvt);


	CMLOrderStateMachine m_mlOrderStateMachine;
	CSgOrderStateMachine m_sgOrderStateMachine;

	int m_maxOrderRef;
	boost::mutex m_mutOrdRefIncr;

};

