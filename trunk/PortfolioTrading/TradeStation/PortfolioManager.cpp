#include "StdAfx.h"
#include "PortfolioManager.h"
#include "globalmembers.h"

#include <boost/format.hpp>


CPortfolioManager::CPortfolioManager(void):
m_quoteAggregator(NULL)
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
	portfolio->SetManager(this);

	logger.Info(boost::str(boost::format("Portfolio(%s) added") % pid.c_str()));
}

void CPortfolioManager::Remove( const string& portfId )
{
	m_mapPortfolios.erase(portfId);
}
