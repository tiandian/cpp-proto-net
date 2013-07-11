#pragma once

#include "Strategy.h"
#include "Leg.h"
#include "entity/message.pb.h"
#include "entity/quote.pb.h"
#include "entity/trade.pb.h"

#include <boost/chrono.hpp>
#include <boost/thread.hpp>

class CQuoteRepositry;
class CQuoteFetcher;
class CAvatarClient;

class CPortfolio
{
public:
	CPortfolio(CAvatarClient* client, const entity::PortfolioItem& srcPortfolioItem);
	~CPortfolio(void);

	const string& ID(){ return m_portfolioItem.id(); }
	int Quantity(){ return m_portfolioItem.quantity(); }
	int Count(){ return m_legs.size(); }

	vector<LegPtr>& Legs(){ return m_legs;}
	CLeg* GetLeg(int legId);
	CLeg* GetLeg(const string& symbol);
	
	entity::StrategyType StrategyType(){ return m_strategyType; }
	CStrategy* Strategy(){ return m_strategy.get(); }
	const entity::PortfolioUpdateItem& GetUpdated(){ return m_portfolioUpdate; }

	void SubscribeQuotes(CQuoteRepositry* pQuoteRepo);

	int NewOrderId(string& newId);
	void AddPosition(trade::MultiLegOrder& openOrder);
	void RemovePosition(trade::MultiLegOrder& closeOrder);

private:
	void AddLeg(const entity::LegItem& legItem);
	StrategyPtr CreateStrategy(const entity::StrategyItem& strategyItem);
	
	void OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);
	
	void GetLegUpdate();
	void GetStatisticsUpdate();
	void PushUpdate();

	// backup PortfolioItem which this is created from
	entity::PortfolioItem m_portfolioItem;
	entity::PortfolioUpdateItem m_portfolioUpdate;
	StrategyPtr m_strategy;
	entity::StrategyType m_strategyType;
	vector<LegPtr> m_legs;

	// for quote subscription
	CQuoteRepositry* m_pQuoteRepo;
	vector<CQuoteFetcher*> m_quoteFetcherVec;

	// statistics
	int m_totalOpenTimes;
	int m_totalCloseTimes;
	int m_currentPosition;
	int m_cancelTimes;
	int m_serialOrderId;
	boost::mutex m_mutStat;

	CAvatarClient* m_avatar;
};

typedef boost::shared_ptr<CPortfolio> PortfolioPtr;

