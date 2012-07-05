#include "StdAfx.h"
#include "ClientAgent.h"
#include "Portfolio.h"

CClientAgent::CClientAgent(void)
{
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

bool CClientAgent::TradeConnect( const std::string& address, const std::string& streamDir )
{
	m_tradeAddress = address;
	return m_tradeAgent.Open(m_tradeAddress, streamDir);
}

bool CClientAgent::TradeLogin( const string& brokerId, const string& userId, const string& password )
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

bool CClientAgent::QuoteLogin( const string& brokerId, const string& userId, const string& password )
{
	return m_quoteAgent.Login(brokerId, userId, password);
}

void CClientAgent::QuoteLogout()
{
	m_quoteAgent.Logout();
}

bool CClientAgent::QuoteConnect( const std::string& address, const std::string& streamDir )
{
	m_quoteAddress = address;
	return m_quoteAgent.Open(m_quoteAddress, streamDir);
}

void CClientAgent::QuoteDisconnect()
{
	m_quoteAgent.Logout();
	m_quoteAgent.Close();
}


