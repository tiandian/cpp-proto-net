#include "stdafx.h"
#include "ManualStrategy.h"


CManualStrategy::CManualStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
{
	Apply(strategyItem, false);
}


CManualStrategy::~CManualStrategy()
{
}

void CManualStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	m_retryTimes = strategyItem.retrytimes();
}

void CManualStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{

}

void CManualStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{

}
