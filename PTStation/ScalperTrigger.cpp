#include "StdAfx.h"
#include "ScalperTrigger.h"
#include "globalmembers.h"
#include "SettingChangeTrace.h"

CScalperTrigger::CScalperTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
	, m_threshold(0)
{
	Apply(triggerItem);
}


CScalperTrigger::~CScalperTrigger(void)
{
}

void CScalperTrigger::Apply( const entity::TriggerItem& triggerItem )
{
	if(m_threshold > 0)
	{
		PrintDoubleChange("Threshold", m_threshold, triggerItem.sc_threshold());
	}

	m_threshold = triggerItem.sc_threshold();
}

bool CScalperTrigger::OnTest( double val )
{
#ifdef LOG_FOR_TRADE
	logger.Debug(boost::str(boost::format("Scalper Trigger Testing %.2f vs %.2f") % val % m_threshold));
#endif

	return DoubleGreaterEqual(val, m_threshold);
}
