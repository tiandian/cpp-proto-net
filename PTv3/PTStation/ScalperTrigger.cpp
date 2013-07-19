#include "StdAfx.h"
#include "ScalperTrigger.h"


CScalperTrigger::CScalperTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}


CScalperTrigger::~CScalperTrigger(void)
{
}

void CScalperTrigger::Apply( const entity::TriggerItem& triggerItem )
{
	m_threshold = triggerItem.sc_threshold();
}
