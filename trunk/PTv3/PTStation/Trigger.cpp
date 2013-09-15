#include "StdAfx.h"
#include "Trigger.h"


CTrigger::CTrigger(const entity::TriggerItem& triggerItem)
{
	m_strategy = triggerItem.strategy();
	m_name = triggerItem.name();
	m_enabled = triggerItem.enabled();
}


CTrigger::~CTrigger(void)
{
}

bool CTrigger::Test( double val )
{
	if(IsEnabled())
		return OnTest(val);

	return false;
}

bool CTrigger::Test( const double vals[], int size )
{
	if(IsEnabled())
		return OnTest(vals, size);
	return false;
}
