#pragma once

#include "Strategy.h"

class CPriceBarDataProxy;

class CTechAnalyStrategy : public CStrategy
{
public:
	CTechAnalyStrategy(const entity::StrategyItem& strategyItem);
	virtual ~CTechAnalyStrategy(void);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	void Start();
	void Stop();

private:


	int m_precision;
	CPriceBarDataProxy* m_pDataProxy;
};

