#pragma once

#include "QuoteAgent.h"
#include "TradeAgent.h"
#include "QuoteAggregator.h"
#include "StrategyManager.h"
#include "PortfolioManager.h"
#include "ManualOrderPlacer.h"
#include "OrderProcessor.h"
#include "QuoteListener.h"
#include "PortfolioOrderHelper.h"
#include "../../APInvoke/APInvokeNative/APInvokeNative.h"

#include <string>
#include <vector>

using namespace std;

class CClientAgent : public CQuoteListener
{
public:
	CClientAgent(void);
	~CClientAgent(void);

	void SetSession(Session* pSession) 
	{ 
		m_pSession = pSession; 
		SetClientStatus( m_pSession != NULL );
	}

	void Add(entity::PortfolioItem* portfolioItem);
	void Remove(const string& pid);
	void RegQuote(vector<string>& symbols);
	void PortfolioOpenPosition(const string& pid, int quantity);
	void PortfolioClosePosition(const string& pid, int quantity);

	void OnQuoteRecevied(boost::shared_ptr<entity::Quote>& pQuote);
	void OnPortfolioUpdated(entity::PortfolioItem* portfolioItem);
	void OnMultiLegOrderUpdated(trade::MultiLegOrder* order);

	std::string QuoteAddress() const { return m_quoteAddress; }
	boost::tuple<bool, string> QuoteConnect(const std::string& address, const std::string& streamDir);
	void QuoteDisconnect();

	boost::tuple<bool, string> TradeLogin(const string& brokerId, const string& userId, const string& password);
	void TradeLogout();

	std::string TradeAddress() const { return m_tradeAddress; }
	boost::tuple<bool, string> TradeConnect(const std::string& address, const std::string& streamDir);
	void TradeDisconnect();

	boost::tuple<bool, string> QuoteLogin(const string& brokerId, const string& userId, const string& password);
	void QuoteLogout();

private:

	void SetClientStatus(bool connected) { m_clientConnected = connected; }

	CQuoteAgent			m_quoteAgent;
	CQuoteAggregator	m_quoteAggregator;
	CTradeAgent			m_tradeAgent;
	CStrategyManager	m_strategyMgr;
	CPortfolioManager	m_portfolioMgr;
	CManualOrderPlacer	m_orderPlacer;
	COrderProcessor		m_orderProcessor;

	string m_quoteAddress;
	string m_tradeAddress;
	string m_brokerId;
	string m_userId;

	Session* m_pSession;

	bool m_clientConnected;
};

