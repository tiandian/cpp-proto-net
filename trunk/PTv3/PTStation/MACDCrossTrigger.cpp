#include "StdAfx.h"
#include "MACDCrossTrigger.h"


CMACDCrossOpenTrigger::CMACDCrossOpenTrigger( const entity::TriggerItem& triggerItem )
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}

bool CMACDCrossOpenTrigger::OnTest( const double vals[], int size )
{
	// in case fast and slow has same sign
	return (vals[0] * vals[1]) > 0;
}

CMACDCrossCloseTrigger::CMACDCrossCloseTrigger( const entity::TriggerItem& triggerItem )
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}

bool CMACDCrossCloseTrigger::OnTest( const double vals[], int size )
{
	// in case fast and slow has different sign
	return (vals[0] * vals[1]) < 0;
}
