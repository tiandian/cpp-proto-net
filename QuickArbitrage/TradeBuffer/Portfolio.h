#pragma once

#include "Leg.h"
#include "Trigger.h"
#include "protobuf_gen/quote.pb.h"

#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/uuid/uuid.hpp>

typedef std::vector< boost::shared_ptr<CLeg> > LegVector;
typedef boost::shared_ptr<CTrigger> TriggerPtr;

class CPortfolio
{
public:
	CPortfolio(void);
	CPortfolio(const boost::uuids::uuid& pid);
	~CPortfolio(void);

	const LegVector& GetLegs(){ return m_legs; }
	CLeg* AddLeg()
	{
		boost::shared_ptr<CLeg> leg(new CLeg(this));
		m_legs.push_back(leg);
		return leg.get();
	}

	int GetQuantity(){ return m_quantity; }
	void SetQuantity(int quantity){ m_quantity = quantity; }

	double GetDiff();

	const boost::uuids::uuid& GetID() {return m_portfolioID;}

	bool GetIsAuto(){ return m_autoRunning; }
	void SetAuto(bool autoFlag);

	void SetEntryTrigger(TriggerPtr& trigger) { m_entryTrigger = trigger; }
	void SetExitTrigger(TriggerPtr& trigger) { m_exitTrigger = trigger; }
	void SetStopLossTrigger(TriggerPtr& trigger) { m_stopLossTrigger = trigger; }

	void UpdateQuote(CTP::Quote* quote);
private:
	
	LegVector m_legs;
	int m_quantity;

	boost::uuids::uuid m_portfolioID;

	TriggerPtr m_entryTrigger;
	TriggerPtr m_exitTrigger;
	TriggerPtr m_stopLossTrigger;

	bool m_autoRunning;
};

