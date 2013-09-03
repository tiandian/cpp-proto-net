#pragma once

#include "TechAnalyStrategy.h"

class CAvatarClient;

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
	void CalculateAngel();

	double m_stdHist;
	double m_angel;
};

