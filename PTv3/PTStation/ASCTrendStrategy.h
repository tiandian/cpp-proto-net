#pragma once

#include "TechAnalyStrategy.h"
#include "WillRDataSet.h"
#include "WATRStopDataSet.h"

class CAvatarClient;
class CPortfolioTrendOrderPlacer;
class COHLCRecordSet;

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
	void ClosePosition(CPortfolio* pPortfolio, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText);

	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);

private:

	entity::PosiDirectionType TestForOpen(entity::Quote* pQuote, double wr, double hi, double lo, double trend);
	bool TestForClose(entity::PosiDirectionType direction, entity::Quote* pQuote, double stopPx, double extraWatr = 0.0);
	bool IsPreBarOpenCorrect(entity::PosiDirectionType direction, double preWR);
	double GetNearStopLoss(entity::PosiDirectionType direction, COHLCRecordSet* ohlcSet, int pos);
	bool IfNotBreakoutPreceding(CPortfolio* pPortfolio, entity::PosiDirectionType direction, COHLCRecordSet* ohlcSet, int currentPos);
	void SetRisk(int risk);

	int m_period;
	int m_riskParam;
	int m_avgPeriodParam;
	int m_boLengthParam;
	double m_X1;
	double m_X2;
	int m_forceCloseOffset;

	bool m_marketOpen;
	int m_lastOpenBarIdx;
	int m_lastCloseBarIdx;
	bool m_isRealSignal;
	bool m_allowFakeSignal;
	entity::PosiDirectionType m_lastPositionOffset;
	double m_initStopPx;

	double m_williamsR;
	double m_watr;
	double m_stopPx;
	double m_donchianHi;
	double m_donchianLo;

	WillRDataSetPtr m_willRIndicatorSet;
	WATRStopDataSetPtr m_watrStopIndSet;

	boost::mutex m_mut;
};

