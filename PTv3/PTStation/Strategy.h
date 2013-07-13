#pragma once

#include "Trigger.h"
#include "entity/message.pb.h"
#include "entity/quote.pb.h"
#include "entity/trade.pb.h"

#include <boost/chrono.hpp>

class CPortfolio;

class CStrategy
{
public:
	CStrategy(const entity::StrategyItem& strategyItem);
	virtual ~CStrategy(void);

	int RetryTimes(){ return m_retryTimes; }
	int OpenTimeout(){ return m_openTimeout; }

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);
	
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder);

protected:

	void AddProfit(CPortfolio* pPortfolio, double profit);
	void SetAvgCost(CPortfolio* pPortfolio, double avgCost);
	int IncrementOpenTimes( CPortfolio* pPortfolio, int qty);
	int IncrementCloseTimes( CPortfolio* pPortfolio, int qty);
	int PositionQuantity(CPortfolio* pPortfolio);
	double AvgCost(CPortfolio* pPortfolio);

	vector<TriggerPtr> m_triggers;
	entity::StrategyType m_type;
	bool m_running;
	int m_retryTimes;
	int m_openTimeout;
};

typedef boost::shared_ptr<CStrategy> StrategyPtr;

