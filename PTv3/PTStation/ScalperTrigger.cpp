#include "StdAfx.h"
#include "ScalperTrigger.h"


CScalperTrigger::CScalperTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
{
	m_threshold = triggerItem.sc_threshold();
}


CScalperTrigger::~CScalperTrigger(void)
{
}
