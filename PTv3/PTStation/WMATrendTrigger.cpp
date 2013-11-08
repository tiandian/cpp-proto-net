#include "StdAfx.h"
#include "WMATrendTrigger.h"
#include "globalmembers.h"


CMACDCrossOpenTrigger::CMACDCrossOpenTrigger( const entity::TriggerItem& triggerItem )
	: CTrigger(triggerItem)
{
	Apply(triggerItem);
}

bool CMACDCrossOpenTrigger::OnTest( const double vals[], int size )
{
	// in case fast and slow has same sign
	return (vals[0] * vals[1]) > 0;
}

CMACDCrossCloseTrigger::CMACDCrossCloseTrigger( const entity::TriggerItem& triggerItem )
	: CTrigger(triggerItem)
	, m_direction(entity::NET)
{
	Apply(triggerItem);
}

bool CMACDCrossCloseTrigger::OnTest( const double vals[], int size )
{
	if(size == 2 && m_direction > entity::NET)
	{
		double slow = vals[0];
		double fast = vals[1];
		bool isSlowAngleGood = m_direction == entity::LONG ? slow > 0 : slow < 0;

		if(isSlowAngleGood)
		{
			// fast MACD has different direction than slow
			LOG_DEBUG(logger, boost::str(boost::format("IF Fast MACD has become against Slow MACD? (fast:%.2f, slow:%.2f)") 
				% fast % slow));
			return slow * fast < 0;
		}
		else
		{
			LOG_DEBUG(logger, boost::str(boost::format("Slow MACD has reversed. (slow:%.2f)") % slow));
			return true;
		}
	}
	return false;
}
