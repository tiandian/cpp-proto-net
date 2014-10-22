#pragma once

#include "TechAnalyStrategy.h"
#include "DonchianDataSet.h"
#include "ATRDataSet.h"
#include "StrategyOrderCommand.h"

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

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem){}

	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);

private:

	entity::PosiDirectionType TestForOpen(CPortfolio* pPortfolio, entity::Quote* pQuote, double upperBound, double lowerBound, string* pOutComment );
	bool TestForClose(CPortfolio* pPortfolio, entity::Quote* pQuote, double upperBound, double lowerBound, string* pOutComment, bool* outRevertOffset);

	int m_timeFrame;
	bool m_marketOpen;
	int m_forceCloseOffset;
	
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
	bool m_trending;

	int m_lastBarIdx;
	OrderCommandPtr m_pendingOrdCmd;
	double m_lastCostPx;
	double m_recentStopLossPx;	// only for display
	entity::PosiDirectionType m_lastPosiDirection;
};

