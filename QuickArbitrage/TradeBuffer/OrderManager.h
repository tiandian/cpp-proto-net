#pragma once

#include "QuoteListener.h"
#include "RemoteClient.h"
#include "TradeAgent.h"

#include <string>
#include <vector>

class COrderManager : public QuoteListener
{
public:
	COrderManager(void);
	~COrderManager(void);

	void Initialize();

	bool Register(ClientBase* pClient, const std::string& brokerId, const std::string& userId, const std::string& password);
	void Unregister(std::string& brokerId, std::string& userId);

	ClientBase* GetCurrentClient(){ return m_pClient; }
	void SetCurrentClient(ClientBase* pClient) { m_pClient = pClient; }

	void Subscribe(std::vector<std::string>& symbols);
	void UnSubscribe();

	void Buy()
	{
		m_tradeAgent.Buy();
	}

	virtual void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///登录请求响应
	void OnRspUserLogin(bool succ, std::string& msg)
	{
		if(m_pClient != NULL)
		{
			m_pClient->OnRegisterResult(succ, msg);
		}

		if(!succ) SetCurrentClient(NULL);
	}

	///投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	void OnRspQryInstrument(){}

	///请求查询资金账户响应
	void OnRspQryTradingAccount(){}

	///请求查询投资者持仓响应
	void OnRspQryInvestorPosition(){}

	///报单录入请求响应
	void OnRspOrderInsert(){}

	///报单操作请求响应
	void OnRspOrderAction(){}


	///报单通知
	void OnRtnOrder(){}

	///成交通知
	void OnRtnTrade(){}
	//////////////////////////////////////////////////////////////////////////
private:

	CTradeAgent	m_tradeAgent;
	
	ClientBase* m_pClient;

};

