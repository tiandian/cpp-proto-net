#pragma once

#include <string>

#include "../Entity/gen/cpp/trade.pb.h"

class CTradeAgentCallback
{
public:
	CTradeAgentCallback(void){}
	virtual ~CTradeAgentCallback(void){}

	///登录请求响应
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID) = 0;

	///报单录入请求响应
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg) = 0;

	///报单操作请求响应
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg) = 0;

	///报单通知
	virtual void OnRtnOrder(trade::Order* order) = 0;

	///成交通知
	virtual void OnRtnTrade(trade::Trade* pTrade) = 0;

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	virtual void OnRspQryInstrument(){}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo){}

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(){}
};