#pragma once

#include "TechAnalyStrategy.h"
#include "WillRDataSet.h"

class CAvatarClient;
class CPortfolioTrendOrderPlacer;

class CASCTrendStrategy : public CTechAnalyStrategy
{
public:
	CASCTrendStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CASCTrendStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening, const char* noteText);
	void ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText);

	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);

private:

	int m_period;
	int m_riskParam;
	int m_avgPeriodParam;
	int m_boLengthParam;

	bool m_marketOpen;
	int m_openAtBarIdx;
	entity::PosiDirectionType m_DirectionOpened;

	int m_ascTrendSignal;
	double m_watrStopVal;
	double m_rsiVal;

	WillRDataSetPtr m_willRIndicatorSet;

	boost::mutex m_mut;
};

