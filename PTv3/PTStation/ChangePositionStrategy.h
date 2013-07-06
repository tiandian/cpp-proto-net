#pragma once
#include "Strategy.h"

class CChangePositionStrategy : public CStrategy
{
public:
	CChangePositionStrategy(const entity::StrategyItem& strategyItem);
	~CChangePositionStrategy(void);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

private:
	void CreateTriggers(const entity::StrategyItem& strategyItem);

	string m_closeLeg;
	entity::PosiDirectionType m_closeLegSide;
};

