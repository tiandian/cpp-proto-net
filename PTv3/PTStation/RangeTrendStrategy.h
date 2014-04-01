#pragma once

#include "TechAnalyStrategy.h"
#include "DonchianDataSet.h"
#include "ATRDataSet.h"

class CAvatarClient;
class CPortfolioTrendOrderPlacer;

class CRangeTrendStrategy : public CTechAnalyStrategy
{
public:
	CRangeTrendStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CRangeTrendStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening, const char* noteText);
	void ClosePosition(CPortfolio* pPortfolio, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText);

	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);

private:

	int m_timeFrame;
	bool m_marketOpen;

	// Strategy parameters
	int m_openPeriod;
	int m_closePeriod;
	double m_stopLossFactor;
	double m_trendFactor;

	DonchianDataSetPtr m_openDonchianDataSet;
	DonchianDataSetPtr m_closeDonchianDataSet;
	ATRDataSetPtr m_atrDataSet;

	boost::mutex m_mut;

	double m_upperBoundOpen;
	double m_lowerBoundOpen;
	double m_upperBoundClose;
	double m_lowerBoundClose;
	double m_currentHigh;
	double m_currentLow;
	double m_NATR;
	double m_StopLoss;
};

