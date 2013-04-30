#include "PortfolioManager.h"
#include "globalmembers.h"

#include <boost/format.hpp>


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
	PortfolioPtr portfWrapper(portfolio);
	m_mapPortfolios.insert(make_pair(pid, portfWrapper));
	
	LOG_INFO(logger, boost::str(boost::format("Portfolio(%s) Added") % pid.c_str()));
}

void CPortfolioManager::Remove( const string& portfId )
{
	m_mapPortfolios.erase(portfId);

	LOG_INFO(logger, boost::str(boost::format("Portfolio(%s) Removed") % portfId));
}
