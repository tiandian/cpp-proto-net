#pragma once
#include "TechAnalyStrategy.h"
#include "Portfolio.h"
#include "DiffRecordSet.h"
#include "BollDataSet.h"

class CPortfolioArbitrageOrderPlacer;

class CArbitrageStrategy : public CTechAnalyStrategy
{
public:
	CArbitrageStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CArbitrageStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);
	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);
	static double CalcMlOrderCost( const trade::MultiLegOrder& openOrder );
	

private:
	static const string& GetAnotherLegSymbol(const string& symb, const vector<LegPtr>& legs);
	static const double CalcBoundaryByTargetGain(double mid, double targetGain, double step, double* outUpper, double* outLower);

	entity::PosiDirectionType GetTradeDirection();
	entity::PosiDirectionType GetFastTradeDirection();
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioArbitrageOrderPlacer* pOrderPlacer, ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void ClosePosition(CPortfolioArbitrageOrderPlacer* pOrderPlacer, ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason);
	void InitForTargetGain(CPortfolio* pPortfolio);

	double m_costDiff;

	int m_timeFrame;
	int m_bollPeriod;
	int m_stdDevMultiplier;
	double m_targetGain;
	double m_minStep;
	bool m_useTargetGain;
	bool m_allowPending;

	double m_lastDiff;
	double m_longDiff;
	double m_longDiffFast;
	int m_longDiffSize;
	double m_shortDiff;
	double m_shortDiffFast;
	int m_shortDiffSize;
	double m_bollTop;
	double m_bollBottom;

	int m_longPosition;
	double m_longAvgCost;
	int m_shortPosition;
	double m_shortAvgCost;

	DiffRecordSetPtr m_diffRecordSet;
	BollDataSetPtr m_bollDataSet;

	boost::mutex m_mut;
};

