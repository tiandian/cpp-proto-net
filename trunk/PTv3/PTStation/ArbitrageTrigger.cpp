#include "StdAfx.h"
#include "ArbitrageTrigger.h"


CArbitrageTrigger::CArbitrageTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
{
	m_condition = triggerItem.ar_condition();
	m_threshold = triggerItem.ar_threshold();
	m_offset = triggerItem.ar_offset();
}


CArbitrageTrigger::~CArbitrageTrigger(void)
{
}
