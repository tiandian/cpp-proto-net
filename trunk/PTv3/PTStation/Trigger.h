#pragma once
#include "entity/message.pb.h"

class CTrigger
{
public:
	CTrigger(const entity::TriggerItem& triggerItem);
	virtual ~CTrigger(void);

	entity::StrategyType Strategy(){ return m_strategy; }
	const string& Name(){ return m_name; }

	virtual bool Test(double val){ return false; }
	bool IsEnabled(){ return m_enabled; }
	void Enable(bool enabled){ m_enabled = enabled; }

protected:
	entity::StrategyType m_strategy;
	string m_name;
	bool m_enabled;
};

typedef boost::shared_ptr<CTrigger> TriggerPtr;

