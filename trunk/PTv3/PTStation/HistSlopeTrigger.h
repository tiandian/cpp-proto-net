#pragma once
#include "Trigger.h"

class CHistSlopeTrigger : public CTrigger
{
public:
	CHistSlopeTrigger(const entity::TriggerItem& triggerItem);
	~CHistSlopeTrigger(void);

	void Apply( const entity::TriggerItem& triggerItem );

private:
	double m_angelThreshold;
	entity::PosiOffsetFlag m_offset;
};

