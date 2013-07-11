#pragma once
#include "entity/message.pb.h"

class CTrigger
{
public:
	CTrigger(const entity::TriggerItem& triggerItem);
	virtual ~CTrigger(void);

	virtual bool Test(double val){ return false; }

protected:
	entity::StrategyType m_strategy;
	string m_name;
	bool m_enabled;
};

typedef boost::shared_ptr<CTrigger> TriggerPtr;

