#pragma once

#include "Strategy.h"

class CScalperStrategy : public CStrategy
{
public:
	CScalperStrategy(const entity::StrategyItem& strategyItem);
	~CScalperStrategy(void);

	double PriceTick(){ return m_priceTick; }

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);

	static double CalcOrderProfit(const trade::MultiLegOrder& openOrder);

private:
	void CreateTriggers(const entity::StrategyItem& strategyItem);

	double m_priceTick;
	entity::DirectionDepends m_caseLE2Tick;
	entity::DirectionDepends m_caseLE3Tick;
	entity::DirectionDepends m_caseGE4Tick;
	entity::DirectionDepends m_caseNoChange;
	entity::StopLossCloseMethods m_stopLossStrategy;

	double m_diff;
	double m_ask;
	int m_askSize;
	double m_bid;
	int m_bidSize;
};

