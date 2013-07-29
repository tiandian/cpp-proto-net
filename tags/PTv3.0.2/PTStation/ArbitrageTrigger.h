#pragma once
#include "Trigger.h"

class CArbitrageTrigger : public CTrigger
{
public:
	CArbitrageTrigger(const entity::TriggerItem& triggerItem);
	~CArbitrageTrigger(void);

	void Apply( const entity::TriggerItem& triggerItem );

private:
	entity::CompareCondition m_condition;
	double m_threshold;
	entity::PosiOffsetFlag m_offset;
};

