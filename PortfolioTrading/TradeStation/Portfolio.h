#pragma once

#include "QuoteListener.h"
#include "../Entity/gen/cpp/message.pb.h"
#include "ArbitrageStrategy.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

using namespace std;

class CLeg;
class CPortfolioManager;

typedef boost::shared_ptr<entity::PortfolioItem> PortfItemPtr;
typedef boost::shared_ptr<CLeg> LegPtr;

class CPortfolio : public CQuoteListener
{
	CPortfolio(void);

public:
	~CPortfolio(void);

	static CPortfolio* Create(entity::PortfolioItem* pPortfItem)
	{
		CPortfolio* pPortf = new CPortfolio();
		pPortf->SetItem(pPortfItem);
		return pPortf;
	}

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

	int NewOrderId(string& newId);

	int Quantity() { return m_innerItem->quantity(); }

	CArbitrageStrategy& Strategy(){ return m_strategy; }

	void EnableStrategy(bool isEnabled) { m_strategyEnabled = isEnabled; }

	void TurnSwitches(bool isAutoOpen, bool isAutoStopGain, bool isAutoStopLoss) 
	{
		m_isAutoOpen = isAutoOpen;
		m_isAutoStopGain = isAutoStopGain;
		m_isAutoStopLoss = isAutoStopLoss;
	}
	void ApplyStrategySetting(const string& name, const string& data);

private:
	void SetItem(entity::PortfolioItem* pPortfItem);

	void PushUpdate();

	vector<LegPtr> m_vecLegs;
	PortfItemPtr m_innerItem;

	CPortfolioManager* m_porfMgr;

	int m_openedOrderCount;
	boost::mutex m_mut;

	CArbitrageStrategy m_strategy;
	bool m_strategyEnabled;
	bool m_isAutoOpen;
	bool m_isAutoStopGain;
	bool m_isAutoStopLoss;
		
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


