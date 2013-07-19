#pragma once
#include "Trigger.h"

class CChangePositionTrigger : public CTrigger
{
public:
	CChangePositionTrigger(const entity::TriggerItem& triggerItem);
	~CChangePositionTrigger(void);

	void Apply(const entity::TriggerItem& triggerItem);

private:
	entity::CompareCondition m_condition;
	double m_threshold;
};

