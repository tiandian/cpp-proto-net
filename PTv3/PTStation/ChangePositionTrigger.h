#pragma once
#include "Trigger.h"

class CChangePositionTrigger : public CTrigger
{
public:
	CChangePositionTrigger(const entity::TriggerItem& triggerItem);
	~CChangePositionTrigger(void);

private:
	entity::CompareCondition m_condition;
	double m_threshold;
};

