#include "StdAfx.h"
#include "PortfolioManager.h"


CPortfolioManager::CPortfolioManager(void)
{
}


CPortfolioManager::~CPortfolioManager(void)
{
}

void CPortfolioManager::AddPortfolio( const entity::PortfolioItem& portfItem )
{
	PortfolioPtr portf(new CPortfolio(portfItem));
	m_portfolios.push_back(portf);

}
