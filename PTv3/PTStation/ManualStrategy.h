#pragma once

#include "Strategy.h"

class CPortfolioTrendOrderPlacer;

class CManualStrategy : public CStrategy
{
public:
	CManualStrategy(const entity::StrategyItem& strategyItem);
	~CManualStrategy();

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);
	virtual void OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume);

protected:
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening);
	void ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText);

private:
	void CalcUpdates(entity::Quote* pQuote);
	void ResetUpdates();
	bool MeetCondition(double profit, entity::CompareCondition condition, double target);

	entity::CompareCondition m_stopGainCondition;
	double m_stopGainThreshold;
	entity::CompareCondition m_stopLossCondition;
	double m_stopLossThreshold;
	entity::PosiDirectionType m_positionDirection;

	double m_profit;
	double m_nearHigh;
	double m_nearLow;
	double m_fallback;
	double m_bounce;

	double m_cost;
	
	double m_lastlast;
	double m_tmpHigh;
	double m_tmpLow;

	boost::mutex m_mut;

};

