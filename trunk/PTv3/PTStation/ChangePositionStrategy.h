#pragma once
#include "ArbitrageStrategy.h"

class CChangePositionStrategy : public CArbitrageStrategy
{
public:
	CChangePositionStrategy(const entity::StrategyItem& strategyItem);
	~CChangePositionStrategy(void);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	
protected:
	void CreateTriggers(const entity::StrategyItem& strategyItem);

private:
	string m_closeLeg;
	entity::PosiDirectionType m_closeLegSide;
};

