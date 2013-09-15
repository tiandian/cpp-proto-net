#include "StdAfx.h"
#include "HistSlopeTrigger.h"
#include "DoubleCompare.h"


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
	m_offset = triggerItem.hs_offset();
	m_fastAngleThreshold = triggerItem.hs_fastanglethreshold();
	m_slowAngleThreshold = triggerItem.hs_slowanglethreshold();
}

bool CHistSlopeTrigger::OnTest( const double vals[], int size )
{
	if(size == 2)
	{
		double fastAngle = fabs(vals[0]);
		double slowAngle = fabs(vals[1]);

		if(m_offset == entity::OPEN)
		{
			if(DoubleGreaterEqual(slowAngle, m_slowAngleThreshold)
				&& DoubleGreaterEqual(fastAngle, m_fastAngleThreshold))
			{
				return true;
			}
		}
		else if(m_offset == entity::CLOSE)
		{
			double fa = vals[0];
			double sa = vals[1];

			if((fa > 0 && sa > 0)
				|| (fa < 0 && sa < 0))
			{
				// 5 min <= 45
				if(DoubleLessEqual(slowAngle, m_fastAngleThreshold))
				{
					// 1 min get flat
					if(DoubleLessEqual(fastAngle, 1))
						return true;
				}
			}
			else // 5 min slope direction different than 1 min
			{
				if(DoubleGreaterEqual(fastAngle, m_fastAngleThreshold))
					return true;
			}
		}
	}
	return false;
}

CHistSlopeTrailingStop::CHistSlopeTrailingStop( const entity::TriggerItem& triggerItem ) 
	: CTrigger(triggerItem)
{

}

void CHistSlopeTrailingStop::Apply( const entity::TriggerItem& triggerItem )
{
	m_backValue = triggerItem.ts_backvalue();
}

bool CHistSlopeTrailingStop::OnTest( double val )
{
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

