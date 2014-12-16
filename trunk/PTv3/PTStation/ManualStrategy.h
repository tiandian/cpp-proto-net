#pragma once

#include "Strategy.h"

class CManualStrategy : public CStrategy
{
public:
	CManualStrategy(const entity::StrategyItem& strategyItem);
	~CManualStrategy();

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

};

