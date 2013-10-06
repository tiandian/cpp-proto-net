#pragma once

#include "TechAnalyStrategy.h"
#include "MACDDataSet.h"
#include "BollDataSet.h"

class CAvatarClient;
class CPortfolioTrendOrderPlacer;
class CMACDCrossOpenTrigger;
class CMACDCrossCloseTrigger;
class CTrailingStopTrigger; 

class CMACDCrossStrategy : public CTechAnalyStrategy
{
public:
	CMACDCrossStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CMACDCrossStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder){ return 0; }
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder){ return 0; }

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText);

	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);

private:

	entity::PosiDirectionType GetDirection(double slowMacdHist, double lastPx, double bollTop, double bollBottom);

	int m_macdShort;
	int m_macdLong;
	int m_macdM;
	int m_bollM;
	int m_bollP;

	int m_fastPeriod;
	int m_slowPeriod;

	double m_fastHistVal;
	double m_slowHistVal;
	double m_bollTop;
	double m_bollBottom;

	double m_macdHistArr[2];

	bool m_marketOpen;


	MACDDataSetPtr m_slowPeriodIndicatorSet;
	MACDDataSetPtr m_fastPeriodIndicatorSet;
	BollDataSetPtr m_fastBollIndicatorSet;

	CMACDCrossOpenTrigger* m_pOpenTrigger;
	CMACDCrossCloseTrigger* m_pCloseTrigger;
	CTrailingStopTrigger* m_pTrailingStopTrigger; 

	boost::mutex m_mut;
};

