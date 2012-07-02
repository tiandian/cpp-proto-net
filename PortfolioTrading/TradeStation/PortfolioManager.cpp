#include "StdAfx.h"
#include "PortfolioManager.h"


CPortfolioManager::CPortfolioManager(void)
{
}


CPortfolioManager::~CPortfolioManager(void)
{
}

CPortfolio* CPortfolioManager::Get( const string& portfId )
{
	PortfolioMapIter portIter = m_mapPortfolios.find(portfId);
	if(portIter != m_mapPortfolios.end())
		return (portIter->second).get();

	return NULL;
}

void CPortfolioManager::Add( CPortfolio* portfolio )
{
	const string& pid = portfolio->ID();
	m_mapPortfolios.insert(make_pair(pid, PortfolioPtr(portfolio)));
}

void CPortfolioManager::Remove( const string& portfId )
{
	m_mapPortfolios.erase(portfId);
}
