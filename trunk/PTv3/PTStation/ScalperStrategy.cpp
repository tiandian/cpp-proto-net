#include "StdAfx.h"
#include "ScalperStrategy.h"
#include "ScalperTrigger.h"

CScalperStrategy::CScalperStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
{
	m_priceTick = strategyItem.sc_pricetick();
	m_caseLE2Tick = strategyItem.sc_casele2tick();
	m_caseLE3Tick = strategyItem.sc_casele3tick();
	m_caseGE4Tick = strategyItem.sc_casege4tick();
	m_caseNoChange = strategyItem.sc_casenochange();
	m_openTimeout = strategyItem.sc_opentimeout();
	m_stopLossStrategy = strategyItem.sc_stoplossstrategy();

	CreateTriggers(strategyItem);
}


CScalperStrategy::~CScalperStrategy(void)
{
}

void CScalperStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	if(strategyItem.triggers_size() > 0)
	{
		m_triggers.push_back(TriggerPtr(new CScalperTrigger(strategyItem.triggers(0))));
	}
}
