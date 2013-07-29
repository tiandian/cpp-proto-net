#include "StdAfx.h"
#include "ChangePositionTrigger.h"


CChangePositionTrigger::CChangePositionTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}


CChangePositionTrigger::~CChangePositionTrigger(void)
{
}

void CChangePositionTrigger::Apply( const entity::TriggerItem& triggerItem )
{
	m_condition = triggerItem.cp_condition();
	m_threshold = triggerItem.cp_threshold();
}
