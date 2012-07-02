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

bool CClientAgent::ConnectTrade( const string& brokerId, const string& userId, const string& password )
{
	m_quoteAgent.Open(m_quoteAddress);

	m_quoteAgent.Login(brokerId, userId, password);

	return false;
}

void CClientAgent::DisconnectTrade()
{
	m_quoteAgent.Logout();

	m_quoteAgent.Close();
}

bool CClientAgent::ConnectQuote( const string& brokerId, const string& userId, const string& password )
{
	m_tradeAgent.Open(m_tradeAddress);

	m_tradeAgent.Login(brokerId, userId, password);

	return false;
}

void CClientAgent::DisconnectQuote()
{
	m_tradeAgent.Logout();

	m_tradeAgent.Close();
}
