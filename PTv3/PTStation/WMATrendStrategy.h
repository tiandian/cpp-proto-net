#pragma once

#include "TechAnalyStrategy.h"
#include "WMATrendDataSet.h"

class CAvatarClient;
class CPortfolioTrendOrderPlacer;
class CTrailingStopTrigger;

class CWMATrendStrategy : public CTechAnalyStrategy
{
public:
	CWMATrendStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CWMATrendStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening);
	void ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText);

	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);

private:

	entity::PosiDirectionType GetDirection(double fastVal, double slowVal);

	int m_wmaParam;
	int m_maN;
	int m_period;
	
	double m_arrLine[2];
	bool m_marketOpen;
	int m_openAtBarIdx;
	entity::PosiDirectionType m_DirectionOpened;

	WMATrendSetPtr m_trendIndicatorSet;
	CTrailingStopTrigger* m_pTrailingStopTrigger; 

	boost::mutex m_mut;
};

