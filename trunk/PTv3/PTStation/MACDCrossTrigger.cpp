#include "StdAfx.h"
#include "MACDCrossTrigger.h"


CMACDCrossOpenTrigger::CMACDCrossOpenTrigger( const entity::TriggerItem& triggerItem )
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}

bool CMACDCrossOpenTrigger::OnTest( const double vals[], int size )
{
	return false;
}

CMACDCrossCloseTrigger::CMACDCrossCloseTrigger( const entity::TriggerItem& triggerItem )
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}

bool CMACDCrossCloseTrigger::OnTest( const double vals[], int size )
{
	return false;
}
