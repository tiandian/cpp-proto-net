#pragma once

#include "entity/message.pb.h"
#include "entity/quote.pb.h"
#include "multilegorderptr.h"
#include "PortfolioScalperOrderPlacer.h"

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

using namespace std;

class CLeg;
class CClientAgent;
class CDiffStrategy;
class CQuoteRepositry;
class CQuoteFetcher;
class CScalperStrategy2;

typedef boost::shared_ptr<entity::PortfolioItem> PortfItemPtr;
typedef boost::shared_ptr<CLeg> LegPtr;

class CPortfolio
{
	CPortfolio(CClientAgent* pClient);

public:
	~CPortfolio(void);

	static CPortfolio* Create(CClientAgent* pClient, entity::PortfolioItem* pPortfItem);

	int Count();
	const string& ID(){ return m_innerItem->id(); }

	CLeg* AddLeg(entity::LegItem* legItem);

	CLeg* GetLeg(int legId);
	CLeg* GetLeg(const string& symbol);

	void SubscribeQuotes(CQuoteRepositry* pQuoteRepo);

	vector<LegPtr>& Legs(){ return m_vecLegs; }

	void OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);

	// Need to be done before remove from manager
	void Cleanup();

	double Difference(){ return m_innerItem->diff(); }
	double LongDiff(){ return m_innerItem->longdiff(); }
	int LongSize(){ return m_innerItem->longsize(); }
	double ShortDiff(){ return m_innerItem->shortdiff(); }
	int ShortSize(){ return m_innerItem->shortsize(); }

	int NewOrderId(string& newId);

	int Quantity() { return m_innerItem->quantity(); }

	CDiffStrategy* Strategy(){ return m_strategy.get(); }
	CPortfolioOrderPlacer& OrderPlacer(){ return m_portfOrdPlacer; }

	bool EnablePrefer();
	bool AutoTracking();
	int RetryTimes();
	int OpenPendingTimeout();
	double PriceTick();

	bool SelfClose(){ return m_selfClose; }
	void SelfClose(bool val){ m_selfClose = val; }

	void EnableStrategy(bool isEnabled);

	void TurnSwitches(bool isAutoOpen, bool isAutoStopGain, bool isAutoStopLoss, bool isAutoTracking, bool enablePrefer);
	void ApplyStrategySetting(const string& name, const string& data);

	void AddPosition(trade::MultiLegOrder& openOrder);
	void RemovePosition(trade::MultiLegOrder& closeOrder);

	bool HasPosition(){ return PositionQuantity() > 0; }
	bool PositionReachLimit(){ return m_innerItem->opentimes() >= m_innerItem->maxposition(); }

	bool IsPlacingOrder() { return m_isPlacingOrder; }
	void BeginPlaceOrder() { m_isPlacingOrder = true; }
	void EndPlaceOrder() { m_isPlacingOrder = false; }

	void VirtualOpen(int qty){ IncrementalOpenTimes(qty); PushUpdate(); }
	void VirtualClose(int qty){ IncrementalCloseTimes(qty); PushUpdate(); }

	void SetQuantity(int qty, int maxQty)
	{ m_innerItem->set_quantity(qty); m_innerItem->set_maxposition(maxQty); PushUpdate(); }

	double AvgCost(){ return m_innerItem->avgcost(); }
	void AvgCost(double avgCost){ return m_innerItem->set_avgcost(avgCost); }

	int PositionQuantity()
	{
		return m_innerItem->currentposition();
	}
	
private:
	void SetItem(entity::PortfolioItem* pPortfItem);

	void UpdatePosition()
	{
		int posiQty = m_innerItem->opentimes() - m_innerItem->closetimes();
		if(posiQty < 0)
			posiQty = 0;
		m_innerItem->set_currentposition(posiQty);
		PushUpdate();
	}

	void PushUpdate();
	void IncrementalOpenTimes(int opened){ m_innerItem->set_opentimes(m_innerItem->opentimes() + opened); UpdatePosition(); }
	void IncrementalCloseTimes(int closed){ m_innerItem->set_closetimes(m_innerItem->closetimes() + closed); UpdatePosition(); }
	void IncrementalOpenCloseTimes(int times)
	{ 
		m_innerItem->set_opentimes(m_innerItem->opentimes() + times);
		m_innerItem->set_closetimes(m_innerItem->closetimes() + times);
		UpdatePosition(); 
	}
	void AddProfit(double val){ m_innerItem->set_profit(m_innerItem->profit() + val); }
	void SetProfit(double val){ m_innerItem->set_profit(val); }

	double CalcMlOrderCost(trade::MultiLegOrder& openOrder);
	double CalcScalpeOrderProfit(trade::MultiLegOrder& openOrder);

	vector<LegPtr> m_vecLegs;
	PortfItemPtr m_innerItem;
	
	CClientAgent* m_pClientAgent;

	CQuoteRepositry* m_pQuoteRepo;
	vector<CQuoteFetcher*> m_quoteFetcherVec;

	int m_openedOrderCount;
	boost::mutex m_mut;
	boost::mutex m_mutQuoting;

	boost::shared_ptr<CDiffStrategy> m_strategy;
	boost::shared_ptr<CScalperStrategy2> m_scalperStrategy;
		
	bool m_selfClose;
	bool m_isPlacingOrder;

	CPortfolioScalperOrderPlacer m_portfOrdPlacer;
};

class CLeg
{
	CLeg(int legId) : 
		m_legId(legId), m_timestamp(0)
	{}

public:
	~CLeg(){}

	friend CLeg* CPortfolio::AddLeg(entity::LegItem* legItem);

	int Id() { return m_legId; }

	const string& Symbol() { return m_pInnerItem->symbol(); }
	entity::PosiDirectionType Side(){ return m_pInnerItem->side(); }
	int Ratio(){ return m_pInnerItem->ratio(); }

	double Last(){ return m_pInnerItem->last(); }
	void UpdateLast(double last){ m_pInnerItem->set_last(last); }

	double Ask(){ return m_pInnerItem->ask(); }
	void UpdateAsk(double ask){ m_pInnerItem->set_ask(ask); }

	int AskSize(){ return m_pInnerItem->asksize(); }
	void UpdateAskSize(int askSize){ m_pInnerItem->set_asksize(askSize); }

	double Bid(){ return m_pInnerItem->bid(); }
	void UpdateBid(double bid){ m_pInnerItem->set_bid(bid); }

	int BidSize(){ return m_pInnerItem->bidsize(); }
	void UpdateBidSize(int bidSize){ m_pInnerItem->set_bidsize(bidSize); }

	entity::LegStatus Status(){ return m_pInnerItem->status(); }
	void UpdateStatus(entity::LegStatus status){ m_pInnerItem->set_status(status); }

	bool IsPreferred() { return m_pInnerItem->ispreferred(); }
	void IsPreferred(bool val) { m_pInnerItem->set_ispreferred(val); }

	entity::LegItem* InnerItem(){ return m_pInnerItem; }
	
	long GetTimestamp(){ return m_timestamp; }
	void UpdateTimestamp();
	bool IsQuoteUpdated(long timestamp);

private:
	void SetInnerItem(entity::LegItem* pItem);
	bool CompareTimestamp(long destTs);
	
	static boost::chrono::steady_clock::time_point m_stBeginTime; 

	int m_legId;
	entity::LegItem* m_pInnerItem;

	long m_timestamp;
	boost::condition_variable m_condQuoteUpdated;
	boost::mutex m_mut;

};


