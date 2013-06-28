#include "StdAfx.h"
#include "Strategy.h"


CStrategy::CStrategy(const entity::StrategyItem& strategyItem)
{
	m_type = strategyItem.type();
	m_running = strategyItem.running();
	m_retryTimes = strategyItem.retrytimes();
}


CStrategy::~CStrategy(void)
{
}
