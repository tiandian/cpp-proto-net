#pragma once

#include "QuoteAgent.h"
#include "TradeAgent.h"
#include "StrategyManager.h"
#include "PortfolioManager.h"
#include "ManualOrderPlacer.h"
#include "OrderProcessor.h"
#include "../Entity/gen/cpp/message.pb.h"

#include <string>

using namespace std;

class CClientAgent
{
public:
	CClientAgent(void);
	~CClientAgent(void);

	void Add(entity::PortfolioItem* portfolioItem);
	void Remove(const string& pid);

	std::string QuoteAddress() const { return m_quoteAddress; }
	bool QuoteConnect(const std::string& address, const std::string& streamDir);
	void QuoteDisconnect();

	bool TradeLogin(const string& brokerId, const string& userId, const string& password);
	void TradeLogout();

	std::string TradeAddress() const { return m_tradeAddress; }
	bool TradeConnect(const std::string& address, const std::string& streamDir);
	void TradeDisconnect();

	bool QuoteLogin(const string& brokerId, const string& userId, const string& password);
	void QuoteLogout();

	void OpenPosition(const string& pid){}
	void ClosePosition(const string& pid){}

private:
	CQuoteAgent			m_quoteAgent;
	CTradeAgent			m_tradeAgent;
	CStrategyManager	m_strategyMgr;
	CPortfolioManager	m_portfolioMgr;
	CManualOrderPlacer	m_orderPlacer;
	COrderProcessor		m_orderProcessor;

	string m_quoteAddress;
	string m_tradeAddress;

};

