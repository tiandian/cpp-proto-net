#pragma once

#include "TradeAgent.h"
#include "QuoteAgent.h"
#include "PortfolioManager.h"
#include "QuoteRepositry.h"

class CAvatarClient : public LogicalConnection
{
public:
	CAvatarClient(const string& sessionId);
	~CAvatarClient(void);

	virtual const char* getKey(){ return m_sessionId.c_str(); }

	const string& Pseudo() const { return m_pseudo; }
	void Pseudo(const string& val) { m_pseudo = val; }

	boost::tuple<bool, string> TradeLogin(const string& address, const string& brokerId, const string& investorId, const string& password);
	void TradeLogout();
	boost::tuple<bool, string> QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& password);
	void QuoteLogout();

	CPortfolioManager& PortfolioManager(){ return m_portfolioMgr; }

private:
	string				m_sessionId;
	string				m_pseudo;
	CTradeAgent			m_tradeAgent;
	CQuoteAgent			m_quoteAgent;
	CQuoteRepositry		m_quoteRepositry;
	CPortfolioManager	m_portfolioMgr;
	
	bool m_tradeLogged;
	bool m_quoteLogged;
};

