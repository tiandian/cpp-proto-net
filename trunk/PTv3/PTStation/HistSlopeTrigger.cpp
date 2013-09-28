#include "StdAfx.h"
#include "HistSlopeTrigger.h"
#include "DoubleCompare.h"
#include "globalmembers.h"


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
			LOG_DEBUG(logger, boost::str(boost::format("HistSlope Testing for OPEN: slowAngle: %.2f >= %.2f, fastAngle: %.2f >= %.2f")
				% slowAngle % m_slowAngleThreshold % fastAngle % m_fastAngleThreshold));
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

			LOG_DEBUG(logger, boost::str(boost::format("HistSlope Testing for CLOSE: slowAngle: %.2f, fastAngle: %.2f")
				% sa % fa));


			if(fa * sa < 0 &&	// fast and slow have different slope direction
				DoubleGreaterEqual(fastAngle, m_fastAngleThreshold))// fast Angle different than slow Angle, AND > 45
			{
				return true;
			}

			// 5 min <= 45
			if(DoubleLessEqual(slowAngle, m_fastAngleThreshold))
			{
				if(fa * sa < 0)
					return true;

				// 1 min get flat
				if(DoubleLessEqual(fastAngle, 2))
				{
					return true;
				}
			}
		}
	}
	return false;
}

CHistSlopeTrailingStop::CHistSlopeTrailingStop( const entity::TriggerItem& triggerItem ) 
	: CTrigger(triggerItem)
	, m_backValue(0.0)
	, m_effectiveStop(0.0)
	, m_lastHigh(0.0)
{
	// disable trailing stop trigger by default
	CTrigger::Enable(false);

	Apply(triggerItem);
}

void CHistSlopeTrailingStop::Apply( const entity::TriggerItem& triggerItem )
{
	m_backValue = triggerItem.ts_backvalue();
}

bool CHistSlopeTrailingStop::OnTest( double val )
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

void CHistSlopeTrailingStop::Enable( double cost, entity::PosiDirectionType direction )
{
	CTrigger::Enable(true);

	m_lastHigh = cost;
	m_direction = direction;
	m_effectiveStop = CalcOffset(cost, -m_backValue);
	
	LOG_DEBUG(logger, boost::str(boost::format("HistSlope trailing stop ENABLED. m_lastHigh:%.2f, m_backValue:%.2f, m_effectiveStop:%.2f")
		% m_lastHigh % m_backValue % m_effectiveStop ));
}

