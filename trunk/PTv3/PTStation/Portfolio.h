#pragma once

#include "Strategy.h"
#include "Leg.h"
#include "entity/message.pb.h"

class CPortfolio
{
public:
	CPortfolio(const entity::PortfolioItem& srcPortfolioItem);
	~CPortfolio(void);

	const string& PortfolioId(){ return m_portfolioItem.id(); }
	const entity::PortfolioUpdateItem& GetUpdated(){ return m_portfolioUpdate; }

private:
	void AddLeg(const entity::LegItem& legItem);
	StrategyPtr CreateStrategy(const entity::StrategyItem& strategyItem);

	// backup PortfolioItem which this is created from
	entity::PortfolioItem m_portfolioItem;
	entity::PortfolioUpdateItem m_portfolioUpdate;
	StrategyPtr m_strategy;
	vector<LegPtr> m_legs;
};

typedef boost::shared_ptr<CPortfolio> PortfolioPtr;

