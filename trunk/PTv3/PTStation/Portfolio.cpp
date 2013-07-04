#include "StdAfx.h"
#include "Portfolio.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"
#include "ScalperStrategy.h"
#include "globalmembers.h"
#include "QuoteFetcher.h"
#include "QuoteRepositry.h"

#include <assert.h>
#include <boost/foreach.hpp>

CPortfolio::CPortfolio(const entity::PortfolioItem& srcPortfolioItem)
{
	// Backup created portfolio item
	m_portfolioItem.CopyFrom(srcPortfolioItem);

	// Create legs
	for(int i = 0; i < srcPortfolioItem.legs_size(); ++i)
	{
		AddLeg(srcPortfolioItem.legs(i));
	}

	// Initialize strategy
	m_strategy = CreateStrategy(srcPortfolioItem.strategy());
}


CPortfolio::~CPortfolio(void)
{
}

void CPortfolio::AddLeg( const entity::LegItem& legItem )
{
	int legCount = (int)m_legs.size();
	LegPtr leg(new CLeg(legCount + 1, legItem));
	m_legs.push_back(leg);
}

StrategyPtr CPortfolio::CreateStrategy( const entity::StrategyItem& strategyItem )
{
	entity::StrategyType straType = strategyItem.type();
	StrategyPtr created;
	switch(straType)
	{
	case entity::ARBITRAGE:
		created = StrategyPtr(new CArbitrageStrategy(strategyItem));
		break;
	case entity::CHANGE_POSITION:
		created = StrategyPtr(new CChangePositionStrategy(strategyItem));
		break;
	case entity::SCALPER:
		created = StrategyPtr(new CScalperStrategy(strategyItem));
		break;
	}
	return created;
}

void CPortfolio::SubscribeQuotes( CQuoteRepositry* pQuoteRepo )
{
	assert(pQuoteRepo != NULL);
	m_pQuoteRepo = pQuoteRepo;

	BOOST_FOREACH(const LegPtr& leg, m_legs)
	{
		const string& symbol = leg->Symbol();
		CQuoteFetcher* pFetcher = m_pQuoteRepo->CreateFetcher(symbol);
		m_quoteFetcherVec.push_back(pFetcher);
		pFetcher->Run(boost::bind(&CPortfolio::OnQuoteRecevied, this, _1, _2));
	}
}

void CPortfolio::OnQuoteRecevied( boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote )
{

}
