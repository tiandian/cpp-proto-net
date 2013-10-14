#pragma once

#include "TechAnalyStrategy.h"
#include "MACDDataSet.h"

class CAvatarClient;
class CHistSlopeOpenTrigger;
class CHistSlopeCloseTrigger;
class CTrailingStopTrigger;
class CPortfolioTrendOrderPlacer;
/*
enum MACDSlopeDirection
{
  NO_DIRECTION, GOING_UP, GOING_DOWN
};
*/
class CHistSlopeStrategy : public CTechAnalyStrategy
{
public:
	CHistSlopeStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CHistSlopeStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);
	void OpenPosition(entity::SlopeDirection slopeDirection, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText);
private:
	double CalculateAngle(double stdHistDiff, double currentHistDiff);

	int m_macdShort;
	int m_macdLong;
	int m_macdM;

	int m_fastPeriod;
	int m_slowPeriod;
	double m_fastStdDiff;
	double m_slowStdDiff;

	double m_fastHistVal;
	double m_fastHistDiff;
	double m_slowHistVal;
	double m_slowHistDiff;
	entity::SlopeDirection m_fastSlopeDirection;
	entity::SlopeDirection m_slowSlopeDirection;

	double m_angleArray[2];
	bool m_positionOpened;
	bool m_marketOpen;

	MACDDataSetPtr m_slowPeriodIndicatorSet;
	MACDDataSetPtr m_fastPeriodIndicatorSet;

	CHistSlopeOpenTrigger* m_pOpenTrigger;
	CHistSlopeCloseTrigger* m_pCloseTrigger;
	CTrailingStopTrigger* m_pTrailingStopTrigger;

	boost::mutex m_mut;
};

