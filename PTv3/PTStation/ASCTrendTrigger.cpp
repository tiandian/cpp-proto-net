#include "StdAfx.h"
#include "ASCTrendTrigger.h"
#include "DoubleCompare.h"
#include "globalmembers.h"

CASCTrendTrigger::CASCTrendTrigger(const entity::TriggerItem& triggerItem)
	: CTrigger(triggerItem)
	, m_direction(entity::NET)
{
	// disable trailing stop trigger by default
	CTrigger::Enable(false);

	Apply(triggerItem);
}

CASCTrendTrigger::~CASCTrendTrigger(void)
{
}

void CASCTrendTrigger::Enable( double cost, entity::PosiDirectionType direction )
{
	CTrigger::Enable(true);

	m_direction = direction;
	m_priceSummit = cost;
}

bool CASCTrendTrigger::OnTest( const double vals[], int size )
{
	double last = vals[0];
	double watr = vals[1];
	
	if (m_direction == entity::LONG)
	{
		if(last > m_priceSummit) m_priceSummit = last;
		double stopVal = m_priceSummit - watr;

		LOG_DEBUG(logger, boost::str(boost::format("ASC Trend trigger testing stop - last:%.2f, WATR:%.2f, StopVal:%.2f")
			% last % watr % stopVal ));

		if (DoubleLessEqual(last, stopVal))
		{
			return true;
		}
	}
	else if (m_direction == entity::SHORT) 
	{
		if (last < m_priceSummit) m_priceSummit = last;
		double stopVal = m_priceSummit + watr;

		LOG_DEBUG(logger, boost::str(boost::format("ASC Trend trigger testing stop - last:%.2f, WATR:%.2f, StopVal:%.2f")
			% last % watr % stopVal ));

		if (DoubleGreaterEqual(last, stopVal))
		{
			return true;
		}
	}
	
	return false;
}
