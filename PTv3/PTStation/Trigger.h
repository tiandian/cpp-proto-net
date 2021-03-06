#pragma once
#include "entity/message.pb.h"

class CTrigger
{
public:
	CTrigger(const entity::TriggerItem& triggerItem);
	virtual ~CTrigger(void);

	virtual void Apply(const entity::TriggerItem& triggerItem) = 0;
	entity::StrategyType Strategy(){ return m_strategy; }
	const string& Name(){ return m_name; }

	bool Test(double val);
	virtual bool OnTest(double val){ return false; }

	bool Test(const double vals[], int size);
	virtual bool OnTest(const double vals[], int size){ return false; }

	bool IsEnabled(){ return m_enabled; }
	void Enable(bool enabled){ m_enabled = enabled; }

protected:
	entity::StrategyType m_strategy;
	string m_name;
	bool m_enabled;
};

typedef boost::shared_ptr<CTrigger> TriggerPtr;

