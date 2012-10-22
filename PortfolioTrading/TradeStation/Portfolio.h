#pragma once

#include "QuoteListener.h"
#include "../Entity/gen/cpp/message.pb.h"
#include "multilegorderptr.h"

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

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
	
	void EnableStrategy(bool isEnabled);

	void TurnSwitches(bool isAutoOpen, bool isAutoStopGain, bool isAutoStopLoss);
	void ApplyStrategySetting(const string& name, const string& data);

	void AddPosition(const MultiLegOrderPtr& openOrder);
	void RemovePosition(const MultiLegOrderPtr& closeOrder);

	int GetPosition(vector<MultiLegOrderPtr>& openedOrders);

private:
	void SetItem(CClientAgent* pClient, entity::PortfolioItem* pPortfItem);

	void PushUpdate();
	void IncrementalOpenTimes(){ m_innerItem->set_opentimes(m_innerItem->opentimes() + 1); }
	void IncrementalCloseTimes(){ m_innerItem->set_closetimes(m_innerItem->closetimes() + 1); }

	vector<LegPtr> m_vecLegs;
	PortfItemPtr m_innerItem;
	map<string, MultiLegOrderPtr> m_openedPosition;

	CPortfolioManager* m_porfMgr;

	int m_openedOrderCount;
	boost::mutex m_mut;

	boost::shared_ptr<CDiffStrategy> m_strategy;
		
	bool m_openOnce;
};

class CLeg
{
	CLeg(int legId) : 
		m_legId(legId)
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

	double Bid(){ return m_pInnerItem->bid(); }
	void UpdateBid(double bid){ m_pInnerItem->set_bid(bid); }

	entity::LegStatus Status(){ return m_pInnerItem->status(); }
	void UpdateStatus(entity::LegStatus status){ m_pInnerItem->set_status(status); }

	bool IsPreferred() { return m_pInnerItem->ispreferred(); }
	void IsPreferred(bool val) { m_pInnerItem->set_ispreferred(val); }

	entity::LegItem* InnerItem(){ return m_pInnerItem; }
	

private:
	void SetInnerItem(entity::LegItem* pItem);

	int m_legId;
	entity::LegItem* m_pInnerItem;

};


