#include "StdAfx.h"
#include "Portfolio.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"
#include "ScalperStrategy.h"

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
