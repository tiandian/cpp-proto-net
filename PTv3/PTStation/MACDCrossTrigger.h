#pragma once

#include "Trigger.h"

#define CrossOpenTriggerName "MACDCrossOpenTrigger"
#define CrossCloseTriggerName "MACDCrossCloseTrigger"
#define CrossTrailingStopTriggerName "MACDCrossTrailingStopTrigger"

class CMACDCrossOpenTrigger : public CTrigger
{
public:
	CMACDCrossOpenTrigger(const entity::TriggerItem& triggerItem);
	~CMACDCrossOpenTrigger(void){}

	void Apply( const entity::TriggerItem& triggerItem ){}

	// Input value should be latest price
	bool OnTest(const double vals[], int size);

	entity::PosiOffsetFlag Offset(){ return entity::OPEN; }
};

class CMACDCrossCloseTrigger : public CTrigger
{
public:
	CMACDCrossCloseTrigger(const entity::TriggerItem& triggerItem);
	~CMACDCrossCloseTrigger(){}

	void Apply( const entity::TriggerItem& triggerItem ){}

	// Input value should be latest price
	bool OnTest(const double vals[], int size);

	entity::PosiOffsetFlag Offset(){ return entity::CLOSE; }
};

