#pragma once
#include "Strategy.h"

class CArbitrageStrategy : public CStrategy
{
public:
	CArbitrageStrategy(const entity::StrategyItem& strategyItem);
	~CArbitrageStrategy(void);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

private:
	void CreateTriggers(const entity::StrategyItem& strategyItem);

	entity::PosiDirectionType m_side;
	double m_lastDiff;
	double m_longDiff;
	int m_longDiffSize;
	double m_shortDiff;
	int m_shortDiffSize;

};

