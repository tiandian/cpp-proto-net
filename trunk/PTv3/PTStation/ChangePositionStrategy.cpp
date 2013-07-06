#include "StdAfx.h"
#include "ChangePositionStrategy.h"
#include "ChangePositionTrigger.h"

CChangePositionStrategy::CChangePositionStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
{
	m_closeLeg = strategyItem.cp_closeleg();
	m_closeLegSide = strategyItem.cp_closelegside();

	CreateTriggers(strategyItem);
}


CChangePositionStrategy::~CChangePositionStrategy(void)
{
}

void CChangePositionStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		TriggerPtr trigger(new CChangePositionTrigger(strategyItem.triggers(i)));
		m_triggers.push_back(trigger);
	}
}

void CChangePositionStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{

}

void CChangePositionStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{

}
