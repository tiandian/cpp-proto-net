#pragma once

#include "tradeagentcallback.h"

#include <string>

using namespace std;

class COrderProcessor :
	public CTradeAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	bool Login(const string& brokerID, const string& userID, const string& password);
	void Logout( const string& brokerID, const string& userID );


	///登录请求响应
	virtual void OnRspUserLogin(){}

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	virtual void OnRspQryInstrument(){}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(){}

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(){}

	///报单录入请求响应
	virtual void OnRspOrderInsert(){}

	///报单操作请求响应
	virtual void OnRspOrderAction(){}


	///报单通知
	virtual void OnRtnOrder(){}

	///成交通知
	virtual void OnRtnTrade(){}
};

