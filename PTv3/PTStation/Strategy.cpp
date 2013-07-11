#include "StdAfx.h"
#include "Strategy.h"
#include "Portfolio.h"


CStrategy::CStrategy(const entity::StrategyItem& strategyItem)
{
	m_type = strategyItem.type();
	m_running = strategyItem.running();
	m_retryTimes = strategyItem.retrytimes();
	m_openTimeout = strategyItem.opentimeout();
}


CStrategy::~CStrategy(void)
{
}

void CStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	vector<LegPtr>& vecLegs = pPortfolio->Legs();
	// update last
	for(vector<LegPtr>::iterator iter = vecLegs.begin(); iter != vecLegs.end(); ++iter)
	{
		if((*iter)->Symbol() == pQuote->symbol())
		{
			(*iter)->UpdateLast(pQuote->last());
			(*iter)->UpdateAsk(pQuote->ask());
			(*iter)->UpdateAskSize(pQuote->ask_size());
			(*iter)->UpdateBid(pQuote->bid());
			(*iter)->UpdateBidSize(pQuote->bid_size());
			(*iter)->UpdateTimestamp();
			break;
		}
	}
}

void CStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{

}
