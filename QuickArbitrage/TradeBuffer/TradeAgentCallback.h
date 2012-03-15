#pragma once

class CTradeAgentCallback
{
public:
	CTradeAgentCallback(void){}
	virtual ~CTradeAgentCallback(void){}

	///登录请求响应
	virtual void OnRspUserLogin() = 0;

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm() = 0;

	///请求查询合约响应
	virtual void OnRspQryInstrument() = 0;

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount() = 0;

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition() = 0;

	///报单录入请求响应
	virtual void OnRspOrderInsert() = 0;

	///报单操作请求响应
	virtual void OnRspOrderAction() = 0;


	///报单通知
	virtual void OnRtnOrder() = 0;

	///成交通知
	virtual void OnRtnTrade() = 0;
	
};