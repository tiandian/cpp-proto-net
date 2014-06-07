#pragma once
#include "TechAnalyStrategy.h"
#include "Portfolio.h"
#include "DiffRecordSet.h"
#include "BollDataSet.h"

class CPortfolioArbitrageOrderPlacer;

class CArbitrageStrategy : public CTechAnalyStrategy
{
public:
	CArbitrageStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CArbitrageStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);

	static double CalcMlOrderCost( const trade::MultiLegOrder& openOrder );

private:
	static const string& GetAnotherLegSymbol(const string& symb, const vector<LegPtr>& legs);

	entity::PosiDirectionType GetTradeDirection();
	void OpenPosition(entity::PosiDirectionType direction, CPortfolioArbitrageOrderPlacer* pOrderPlacer, CPortfolio* pPortfolio, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening);
	void ClosePosition(CPortfolioArbitrageOrderPlacer* pOrderPlacer, CPortfolio* pPortfolio, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment);

	entity::PosiDirectionType m_side;

	int m_timeFrame;
	int m_bollPeriod;
	int m_stdDevTimes;

	double m_lastDiff;
	double m_longDiff;
	int m_longDiffSize;
	double m_shortDiff;
	int m_shortDiffSize;
	double m_bollTop;
	double m_bollBottom;

	DiffRecordSetPtr m_diffRecordSet;
	BollDataSetPtr m_bollDataSet;

	boost::mutex m_mut;
};

