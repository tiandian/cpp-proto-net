#include "StdAfx.h"
#include "ChangePositionTrigger.h"


CChangePositionTrigger::CChangePositionTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
{
	m_condition = triggerItem.cp_condition();
	m_threshold = triggerItem.cp_threshold();
}


CChangePositionTrigger::~CChangePositionTrigger(void)
{
}
