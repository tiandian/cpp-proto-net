#include "StdAfx.h"
#include "HistSlopeTrigger.h"


CHistSlopeTrigger::CHistSlopeTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}


CHistSlopeTrigger::~CHistSlopeTrigger(void)
{
}

void CHistSlopeTrigger::Apply( const entity::TriggerItem& triggerItem )
{
	m_angelThreshold = triggerItem.hs_angelthreshold();
	m_offset = triggerItem.hs_offset();
}

CHistSlopeTrailingStop::CHistSlopeTrailingStop( const entity::TriggerItem& triggerItem ) 
	: CTrigger(triggerItem)
{

}

void CHistSlopeTrailingStop::Apply( const entity::TriggerItem& triggerItem )
{
	m_backValue = triggerItem.ts_backvalue();
}
