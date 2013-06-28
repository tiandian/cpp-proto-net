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
