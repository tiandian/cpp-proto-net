#pragma once

#include "TradeAgentCallback.h"
#include "TradeAgent.h"
#include "multilegorderptr.h"
#include "SequenceOrderSender.h"
#include "OrderResubmitter.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

using namespace std;

typedef boost::function<void(trade::MultiLegOrder*)> PushMultiLegOrderFunc;
typedef boost::function<void( const string&, const string&, trade::Order* legOrd)> PushLegOrderFunc;
typedef boost::function<void(trade::Trade*)> PushTradeFunc;
typedef boost::function<void(const MultiLegOrderPtr&)> PushPortfolioPositionChangeFunc;
typedef boost::function<void(const string&, COrderResubmitter*, bool)> PushPortfolioResubmitterChangeFunc;
typedef boost::shared_ptr<CSequenceOrderSender> OrderSenderPtr;

class COrderProcessor : public CTradeAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void Initialize(CTradeAgent* pTradeAgent);

	void SubmitOrder(MultiLegOrderPtr multilegOrder);
	void SubmitOrder2(MultiLegOrderPtr multilegOrder);
	void CancelOrder(	const std::string& ordRef, 
						const std::string& exchId, 
						const std::string& ordSysId, 
						const std::string& userId,
						const std::string& symbol);

	void CancelOrder(const string& mlOrderId, const string& ordRef);

	void ModifyOrder(const string& mlOrderId, const string& legOrderRef, double limitprice, string* modifiedOrdRef);

	bool QueryAccountInfo(string* outSerializedAcctInfo);

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

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	virtual void OnRspQryInstrument(){}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo);

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void SetPushPortfolioFunc(PushMultiLegOrderFunc funcPushMLOrder);

	void PublishOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void SetPushOrderFunc(PushLegOrderFunc funcPushOrder);

	void PublishTradeUpdate(trade::Trade* pTrade);
	void SetPushTradeFunc(PushTradeFunc funcPushTrade);

	void OnPortfolioPositionChanged(const MultiLegOrderPtr& multilegOrder);
	void SetPushPositionChangeFunc(PushPortfolioPositionChangeFunc funcPushPosiChange);

	void ChangePortfolioResubmitter(const string& portfId, COrderResubmitter* pResubmitter, bool isAdding);
	void SetPushResubmitterChangeFunc(PushPortfolioResubmitterChangeFunc funcResubmitterChange);

	void SubmitOrderToTradeAgent(trade::InputOrder* pOrder, const string& mlOrderId);

private:
	int IncrementalOrderRef(trade::MultiLegOrder* pMlOrder, int maxOrderRef);
	int IncrementalOrderRef(trade::Order* pLegOrd, int maxOrderRef);
	void RemoveFromPending(trade::MultiLegOrder* pMlOrder);
	void SetNonPreferredOrderStatus(
		trade::MultiLegOrder* mlOrder, const string& prefOrdRef,
		trade::OrderStatusType otherStatus);

	map<string, MultiLegOrderPtr> m_pendingMultiLegOrders;
	typedef map<string, MultiLegOrderPtr>::iterator MultiLegOrderIter;
	map<string /* orderRef */, string /* mlOrderId */> m_pendingTicketOrderMap;
	typedef map<string, string>::iterator PendingTktOrdMapIter;
	boost::mutex m_mutTicketOrderMap;
	
	map<string, OrderSenderPtr> m_orderSenderMap;
	typedef map<string, OrderSenderPtr>::iterator OrderSenderMapIter;

	ResubmitterMap m_resubmitterMap;
	
	CTradeAgent* m_pTradeAgent;

	PushMultiLegOrderFunc m_pushMultiOrdFunc;
	PushLegOrderFunc m_pushLegOrderFunc;
	PushTradeFunc m_pushTradeFunc;
	PushPortfolioPositionChangeFunc m_pushPortfPosiChangeFunc;
	PushPortfolioResubmitterChangeFunc m_pushResubmitterChangeFunc;

	int m_maxOrderRef;
	boost::mutex m_mutOrdRefIncr;

	boost::condition_variable m_condQryAcct;
	boost::mutex m_mutQryAcct;
	string m_serializedQryAcctInfo;
};

