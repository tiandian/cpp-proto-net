#include "StdAfx.h"
#include "ClientAgent.h"
#include "Portfolio.h"
#include "PortfolioOrderHelper.h"

#include <boost/shared_ptr.hpp>

CClientAgent::CClientAgent(void):
m_clientConnected(false)
{
	m_quoteAggregator.Initialize(&m_quoteAgent);
}

CClientAgent::~CClientAgent(void)
{
}

void CClientAgent::Add( entity::PortfolioItem* portfolioItem )
{
	// let CPortfolio wrap and hold entity::PortfolioItem instance
	CPortfolio* pPortf = CPortfolio::Create(portfolioItem); 
	m_portfolioMgr.Add(pPortf);
}

void CClientAgent::Remove( const string& pid )
{
	m_portfolioMgr.Remove(pid);
}

boost::tuple<bool, string> CClientAgent::TradeConnect( const std::string& address, const std::string& streamDir )
{
	m_tradeAddress = address;
	return m_tradeAgent.Open(m_tradeAddress, streamDir);
}

boost::tuple<bool, string> CClientAgent::TradeLogin( const string& brokerId, const string& userId, const string& password )
{
	return m_tradeAgent.Login(brokerId, userId, password);
}

void CClientAgent::TradeLogout()
{
	m_tradeAgent.Logout();
}

void CClientAgent::TradeDisconnect()
{
	m_tradeAgent.Logout();
	m_tradeAgent.Close();
}

boost::tuple<bool, string> CClientAgent::QuoteLogin( const string& brokerId, const string& userId, const string& password )
{
	return m_quoteAgent.Login(brokerId, userId, password);
}

void CClientAgent::QuoteLogout()
{
	m_quoteAgent.Logout();
}

boost::tuple<bool, string> CClientAgent::QuoteConnect( const std::string& address, const std::string& streamDir )
{
	m_quoteAddress = address;
	return m_quoteAgent.Open(m_quoteAddress, streamDir);
}

void CClientAgent::QuoteDisconnect()
{
	m_quoteAgent.Logout();
	m_quoteAgent.Close();
}

void CClientAgent::RegQuote( vector<string>& symbols )
{
	if(symbols.size() > 0)
	{
		if(!Registered())
		{
			SetSymbols(symbols);
			m_quoteAggregator.SubscribeQuotes(this);
		}
		else
		{
			m_quoteAggregator.ChangeQuotes(this, symbols);
		}
	}
	else
	{
		m_quoteAggregator.UnsubscribeQuotes(this);
	}
}

void CClientAgent::OnQuoteRecevied( boost::shared_ptr<entity::Quote>& pQuote )
{
	std::string callbackData;
	pQuote->SerializeToString(&callbackData);
	m_pSession->BeginCallback("QuotePush", callbackData);
}

void CClientAgent::PortfolioOpenPosition( const string& pid, int quantity )
{
	// build order
	CPortfolio* portf = m_portfolioMgr.Get(pid);
	boost::shared_ptr<trade::MultiLegOrder> multilegOrder(BuildOrder(portf));
	// send to order processor
	m_orderProcessor.OpenOrder(multilegOrder);
}


