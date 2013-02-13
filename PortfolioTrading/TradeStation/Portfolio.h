#pragma once

#include "QuoteListener.h"
#include "../Entity/gen/cpp/message.pb.h"
#include "multilegorderptr.h"
#include "OrderResubmitter.h"

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

using namespace std;

class CLeg;
class CPortfolioManager;
class CClientAgent;
class CDiffStrategy;

typedef boost::shared_ptr<entity::PortfolioItem> PortfItemPtr;
typedef boost::shared_ptr<CLeg> LegPtr;

class CPortfolio : public CQuoteListener
{
	CPortfolio(void);

public:
	~CPortfolio(void);

	static CPortfolio* Create(CClientAgent* pClient, entity::PortfolioItem* pPortfItem);

	void SetManager(CPortfolioManager* parentMgr);

	int Count();
	const string& ID(){ return m_innerItem->id(); }

	CLeg* AddLeg(entity::LegItem* legItem);

	CLeg* GetLeg(int legId);
	CLeg* GetLeg(const string& symbol);

	vector<LegPtr>& Legs(){ return m_vecLegs; }

	void OnQuoteRecevied(boost::shared_ptr<entity::Quote>& pQuote);

	// Need to be done before remove from manager
	void Cleanup();

	double Difference(){ return m_innerItem->diff(); }
	double LongDiff(){ return m_innerItem->longdiff(); }
	double ShortDiff(){ return m_innerItem->shortdiff(); }

	int NewOrderId(string& newId);

	int Quantity() { return m_innerItem->quantity(); }

	CDiffStrategy* Strategy(){ return m_strategy.get(); }

	bool EnablePrefer();
	bool AutoTracking();
	
	void EnableStrategy(bool isEnabled);

	void TurnSwitches(bool isAutoOpen, bool isAutoStopGain, bool isAutoStopLoss, bool isAutoTracking, bool enablePrefer);
	void ApplyStrategySetting(const string& name, const string& data);

	void AddPosition(const MultiLegOrderPtr& openOrder);
	void RemovePosition(const MultiLegOrderPtr& closeOrder);

	int GetPosition(vector<MultiLegOrderPtr>& openedOrders);

	bool HasPosition(){ return PositionQuantity() > 0; }
	bool PositionReachLimit(){ return PositionQuantity() >= m_innerItem->maxposition(); }

	bool IsPlacingOrder() { return m_isPlacingOrder; }
	void BeginPlaceOrder() { m_isPlacingOrder = true; }
	void EndPlaceOrder() { m_isPlacingOrder = false; }

	void VirtualOpen(int qty){ IncrementalOpenTimes(qty); PushUpdate(); }
	void VirtualClose(int qty){ IncrementalCloseTimes(qty); PushUpdate(); }

	void SetQuantity(int qty, int maxQty)
	{ m_innerItem->set_quantity(qty); m_innerItem->set_maxposition(maxQty); PushUpdate(); }

	double AvgCost(){ return m_innerItem->avgcost(); }
	void AvgCost(double avgCost){ return m_innerItem->set_avgcost(avgCost); }
	
private:
	void SetItem(CClientAgent* pClient, entity::PortfolioItem* pPortfItem);

	int PositionQuantity()
	{
		return m_innerItem->currentposition();
	}

	void UpdatePosition()
	{
		int posiQty = m_innerItem->opentimes() - m_innerItem->closetimes();
		m_innerItem->set_currentposition(posiQty);
		PushUpdate();
	}

	void PushUpdate();
	void IncrementalOpenTimes(int opened){ m_innerItem->set_opentimes(m_innerItem->opentimes() + opened); UpdatePosition(); }
	void IncrementalCloseTimes(int closed){ m_innerItem->set_closetimes(m_innerItem->closetimes() + closed); UpdatePosition(); }
	void AddProfit(double val){ m_innerItem->set_profit(m_innerItem->profit() + val); }

	double CalcMlOrderCost(const MultiLegOrderPtr& openOrder);

	vector<LegPtr> m_vecLegs;
	PortfItemPtr m_innerItem;
	map<string, MultiLegOrderPtr> m_openedPosition;

	CPortfolioManager* m_porfMgr;

	int m_openedOrderCount;
	boost::mutex m_mut;

	boost::shared_ptr<CDiffStrategy> m_strategy;
		
	bool m_openOnce;
	bool m_isPlacingOrder;
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
	
	static boost::posix_time::ptime m_stEpochTime; 

	int m_legId;
	entity::LegItem* m_pInnerItem;

	long m_timestamp;
	boost::condition_variable m_condQuoteUpdated;
	boost::mutex m_mut;
};


