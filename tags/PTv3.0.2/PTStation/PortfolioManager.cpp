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

void CPortfolioManager::AddPortfolio( CAvatarClient* client, const entity::PortfolioItem& portfItem )
{
	PortfolioPtr portf(new CPortfolio(client, portfItem));
	m_portfolios.push_back(portf);
	portf->SubscribeQuotes(m_pQuoteRepositry);
}

CPortfolio* CPortfolioManager::Get( const string& portfolioId )
{
	for(std::vector<PortfolioPtr>::iterator iter = m_portfolios.begin();
		iter != m_portfolios.end(); ++iter)
	{
		if((*iter)->ID() == portfolioId)
		{
			return iter->get();
		}
	}

	return NULL;
}
