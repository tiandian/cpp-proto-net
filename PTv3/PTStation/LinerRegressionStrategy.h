#pragma once

#include "TechAnalyStrategy.h"
#include "LinerRegressionDataSet.h"

class CAvatarClient;
class CPortfolioTrendOrderPlacer;

class CLinerRegressionStrategy : public CTechAnalyStrategy
{
public:
	CLinerRegressionStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CLinerRegressionStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);

protected:
	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);

private:
	static entity::PosiDirectionType GetDirection(double angle);
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote);

	boost::mutex m_mut;
	bool m_marketOpen;

	int m_period;
	int m_number;
	double m_openThreshold;
	double m_closeThreshold;

	double m_weightMidPoint;
	double m_linerRegAngle;
	entity::PosiDirectionType m_direction;
	int m_openAtBarIdx;

	LinerRegSetPtr m_linerRegDataSet;
};

