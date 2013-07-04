#include "StdAfx.h"
#include "PortfolioManager.h"
#include "QuoteRepositry.h"

CPortfolioManager::CPortfolioManager(void)
	: m_pQuoteRepositry(NULL)
{
}


CPortfolioManager::~CPortfolioManager(void)
{
}

void CPortfolioManager::AddPortfolio( const entity::PortfolioItem& portfItem )
{
	PortfolioPtr portf(new CPortfolio(portfItem));
	m_portfolios.push_back(portf);
	portf->SubscribeQuotes(m_pQuoteRepositry);
}
