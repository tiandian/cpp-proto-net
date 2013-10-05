#pragma once
#include "Trigger.h"

#define HistSlopeTriggerName "HistSlopeTrigger"
#define HistSlopeTrailingStopTriggerName "HistSlopeTrailingStopTrigger"

class CHistSlopeTrigger : public CTrigger
{
public:
	CHistSlopeTrigger(const entity::TriggerItem& triggerItem);
	~CHistSlopeTrigger(void);

	void Apply( const entity::TriggerItem& triggerItem );

	// Input value should be latest price
	bool OnTest(const double vals[], int size);

	entity::PosiOffsetFlag Offset(){ return m_offset; }

private:
	double m_fastAngleThreshold;
	double m_slowAngleThreshold;
	entity::PosiOffsetFlag m_offset;
};


