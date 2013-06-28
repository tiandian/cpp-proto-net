#pragma once
#include "Trigger.h"

class CArbitrageTrigger : public CTrigger
{
public:
	CArbitrageTrigger(const entity::TriggerItem& triggerItem);
	~CArbitrageTrigger(void);

private:
	entity::CompareCondition m_condition;
	double m_threshold;
	entity::PosiOffsetFlag m_offset;
};

