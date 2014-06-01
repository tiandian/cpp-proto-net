#include "StdAfx.h"
#include "ChangePositionStrategy.h"
#include "ChangePositionTrigger.h"

CChangePositionStrategy::CChangePositionStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CArbitrageStrategy(strategyItem, pAvatar)
{
	Apply(strategyItem, false);

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
	CArbitrageStrategy::Test(pQuote, pPortfolio, timestamp);
}

void CChangePositionStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	CStrategy::Apply(strategyItem, withTriggers);

	m_closeLeg = strategyItem.cp_closeleg();
	m_closeLegSide = strategyItem.cp_closelegside();
}
