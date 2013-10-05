#include "StdAfx.h"
#include "TrailingStopTrigger.h"
#include "globalmembers.h"


CTrailingStopTrigger::CTrailingStopTrigger( const entity::TriggerItem& triggerItem ) 
	: CTrigger(triggerItem)
	, m_backValue(0.0)
	, m_effectiveStop(0.0)
	, m_lastHigh(0.0)
{
	// disable trailing stop trigger by default
	CTrigger::Enable(false);

	Apply(triggerItem);
}

void CTrailingStopTrigger::Apply( const entity::TriggerItem& triggerItem )
{
	m_backValue = triggerItem.ts_backvalue();
}

bool CTrailingStopTrigger::OnTest( double val )
{
	LOG_DEBUG(logger, boost::str(boost::format("HistSlope trailing stop TESTING: Last:%.2f, m_effectiveStop:%.2f, m_lastHigh:%.2f")
		% val % m_effectiveStop % m_lastHigh));

	if(Compare(val, m_lastHigh))
	{
		m_lastHigh = val;
		m_effectiveStop = CalcOffset(m_lastHigh, -m_backValue);
	}
	else if(!Compare(val, m_effectiveStop))
	{
		// disable this trigger once fired
		CTrigger::Enable(false);

		return true;
	}

	return false;
}

void CTrailingStopTrigger::Enable( double cost, entity::PosiDirectionType direction )
{
	CTrigger::Enable(true);

	m_lastHigh = cost;
	m_direction = direction;
	m_effectiveStop = CalcOffset(cost, -m_backValue);

	LOG_DEBUG(logger, boost::str(boost::format("HistSlope trailing stop ENABLED. m_lastHigh:%.2f, m_backValue:%.2f, m_effectiveStop:%.2f")
		% m_lastHigh % m_backValue % m_effectiveStop ));
}