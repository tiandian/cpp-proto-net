#pragma once

#include "TechAnalyStrategy.h"

class CAvatarClient;

enum MACDSlopeDirection
{
  NO_DIRECTION, GOING_UP, GOING_DOWN
};

class CHistSlopeStrategy : public CTechAnalyStrategy
{
public:
	CHistSlopeStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	~CHistSlopeStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder);

protected:
	virtual void CreateTriggers(const entity::StrategyItem& strategyItem);

private:
	double CalculateAngle(double val1, double val2);

	double m_stdHistDiff;    // standard diff between last 2 bar regarded as angle 45
	double m_angel;
	bool m_positionOpened;
};

