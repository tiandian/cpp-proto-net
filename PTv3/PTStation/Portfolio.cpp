#include "StdAfx.h"
#include "Portfolio.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"
#include "ScalperStrategy.h"
#include "globalmembers.h"
#include "QuoteFetcher.h"
#include "QuoteRepositry.h"
#include "AvatarClient.h"

#include <assert.h>
#include <boost/foreach.hpp>
#include <iostream>

CPortfolio::CPortfolio(CAvatarClient* client, const entity::PortfolioItem& srcPortfolioItem)
	: m_avatar(client)
	, m_pQuoteRepo(NULL)
	, m_strategyType(entity::ARBITRAGE)
{
	// Backup created portfolio item
	m_portfolioItem.CopyFrom(srcPortfolioItem);
	// Initialize portfolio update item
	m_portfolioUpdate.set_id(m_portfolioItem.id());
	m_portfolioUpdate.set_totalopentimes(0);
	m_portfolioUpdate.set_totalclosetimes(0);
	m_portfolioUpdate.set_currentposition(0);
	m_portfolioUpdate.set_canceltimes(0);

	// Create legs
	for(int i = 0; i < srcPortfolioItem.legs_size(); ++i)
	{
		AddLeg(srcPortfolioItem.legs(i));
	}

	// Initialize strategy
	m_strategy = CreateStrategy(srcPortfolioItem.strategy());
	m_portfolioUpdate.set_strategy(m_strategyType);
}


CPortfolio::~CPortfolio(void)
{
}

void CPortfolio::AddLeg( const entity::LegItem& legItem )
{
	// prepare legs in portfolio update item
	entity::LegUpdateItem* legUpdate = m_portfolioUpdate.add_legs();
	legUpdate->set_symbol(legItem.symbol());

	int legCount = (int)m_legs.size();
	LegPtr leg(new CLeg(legCount + 1, legItem, legUpdate));
	m_legs.push_back(leg);
}

StrategyPtr CPortfolio::CreateStrategy( const entity::StrategyItem& strategyItem )
{
	m_strategyType = strategyItem.type();
	StrategyPtr created;
	switch(m_strategyType)
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
	cout << "Quote incoming: " << pQuote->symbol() << ", " << pQuote->last() << ", " << pQuote->update_time() << endl; 
	// Update leg's last with income quote in CStrategy::Test
	m_strategy->Test(pQuote, this, timestamp);

	GetLegUpdate();
	m_strategy->GetStrategyUpdate(&m_portfolioUpdate);
	m_avatar->PublishPortfolioUpdate(m_portfolioUpdate);
}

void CPortfolio::GetLegUpdate()
{
	// update last
	for(vector<LegPtr>::iterator iter = m_legs.begin(); iter != m_legs.end(); ++iter)
	{
		(*iter)->GetUpdated();
	}
}

