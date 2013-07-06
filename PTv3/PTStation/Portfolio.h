#pragma once

#include "Strategy.h"
#include "Leg.h"
#include "entity/message.pb.h"
#include "entity/quote.pb.h"

#include <boost/chrono.hpp>

class CQuoteRepositry;
class CQuoteFetcher;
class CAvatarClient;

class CPortfolio
{
public:
	CPortfolio(CAvatarClient* client, const entity::PortfolioItem& srcPortfolioItem);
	~CPortfolio(void);

	const string& PortfolioId(){ return m_portfolioItem.id(); }
	vector<LegPtr>& Legs(){ return m_legs;}
	const entity::PortfolioUpdateItem& GetUpdated(){ return m_portfolioUpdate; }

	void SubscribeQuotes(CQuoteRepositry* pQuoteRepo);

private:
	void AddLeg(const entity::LegItem& legItem);
	StrategyPtr CreateStrategy(const entity::StrategyItem& strategyItem);

	void OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);
	void GetLegUpdate(entity::Quote* pQuote);

	// backup PortfolioItem which this is created from
	entity::PortfolioItem m_portfolioItem;
	entity::PortfolioUpdateItem m_portfolioUpdate;
	StrategyPtr m_strategy;
	vector<LegPtr> m_legs;

	// for quote subscription
	CQuoteRepositry* m_pQuoteRepo;
	vector<CQuoteFetcher*> m_quoteFetcherVec;

	CAvatarClient* m_avatar;
};

typedef boost::shared_ptr<CPortfolio> PortfolioPtr;

