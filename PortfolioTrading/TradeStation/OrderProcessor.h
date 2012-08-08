#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "TradeAgentCallback.h"
#include "TradeAgent.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

using namespace std;

typedef boost::shared_ptr<trade::MultiLegOrder> MultiLegOrderPtr;

class COrderProcessor : public CTradeAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void Initialize(CTradeAgent* pTradeAgent);

	void OpenOrder(MultiLegOrderPtr multilegOrder);
	void CloseOrder(const string& orderId);
	void CancelOrder(const string& orderId);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///登录请求响应
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///报单录入请求响应
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg){}

	///报单操作请求响应
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg){}

	///报单通知
	virtual void OnRtnOrder(trade::Order* order){}

	///成交通知
	virtual void OnRtnTrade(trade::Trade* pTrade){}

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	virtual void OnRspQryInstrument(){}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(){}

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

private:
	map<string, MultiLegOrderPtr> m_pendingMultiLegOrders;

	map<int, string> m_pendingTicketOrderMap;
	 
	CTradeAgent* m_pTradeAgent;
};

