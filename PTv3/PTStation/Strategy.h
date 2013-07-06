#pragma once

#include "Trigger.h"
#include "entity/message.pb.h"
#include "entity/quote.pb.h"

#include <boost/chrono.hpp>

class CPortfolio;

class CStrategy
{
public:
	CStrategy(const entity::StrategyItem& strategyItem);
	virtual ~CStrategy(void);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);
	
protected:
	vector<TriggerPtr> m_triggers;
	entity::StrategyType m_type;
	bool m_running;
	int m_retryTimes;
};

typedef boost::shared_ptr<CStrategy> StrategyPtr;

