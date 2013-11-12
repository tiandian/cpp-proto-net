#pragma once

#include "Trigger.h"
#include "entity/message.pb.h"
#include "entity/quote.pb.h"
#include "entity/trade.pb.h"

#include <boost/chrono.hpp>
#include <boost/atomic.hpp>

class CPortfolio;

class CStrategy
{
public:
	CStrategy(const entity::StrategyItem& strategyItem);
	virtual ~CStrategy(void);

	int RetryTimes(){ return m_retryTimes; }
	int OpenTimeout(){ return m_openTimeout; }
	vector<TriggerPtr>& Triggers() { return m_triggers; }

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	void ApplyTrigger(const entity::StrategyItem& strategyItem);
	virtual void Cleanup(){}

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);
	
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder);

	bool IsRunning(){ return m_running.load(boost::memory_order_acquire); }
	virtual void Start(){ 
		m_running.store(true, boost::memory_order_release); 
		ResetForceOpen();
		ResetForceClose();
	}
	virtual void Stop(){ m_running.store(false, boost::memory_order_release); }

	static double CalcOrderProfit(const trade::MultiLegOrder& openOrder);

	void SetForceOpen(){ m_forceOpening.store(true, boost::memory_order_release); }
	void ResetForceOpen(){ m_forceOpening.store(false, boost::memory_order_release); }
	bool IsForceOpening(){ return m_forceOpening.load(boost::memory_order_acquire); }

	void SetForceClose(){ m_forceClosing.store(true, boost::memory_order_release); };
	void ResetForceClose(){ m_forceClosing.store(false, boost::memory_order_release);};
	bool IsForceClosing(){ return m_forceClosing.load(boost::memory_order_acquire); };

protected:

	void AddProfit(CPortfolio* pPortfolio, double profit);
	void SetAvgCost(CPortfolio* pPortfolio, double avgCost);
	int IncrementOpenTimes( CPortfolio* pPortfolio, int qty);
	int IncrementCloseTimes( CPortfolio* pPortfolio, int qty);
	int PositionQuantity(CPortfolio* pPortfolio);
	double AvgCost(CPortfolio* pPortfolio);

	vector<TriggerPtr> m_triggers;
	entity::StrategyType m_type;
	boost::atomic<bool> m_running;
	int m_retryTimes;
	int m_openTimeout;

	boost::atomic<bool> m_forceOpening;
	boost::atomic<bool> m_forceClosing;
};

typedef boost::shared_ptr<CStrategy> StrategyPtr;
typedef vector<TriggerPtr>::iterator TriggerIter;


