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

bool CHistSlopeOpenTrigger::OnTest( const double vals[], int size )
{
	if(size == 2)
	{
		double fastAngle = fabs(vals[0]);
		double slowAngle = fabs(vals[1]);

		if(m_offset == entity::OPEN)
		{
			LOG_DEBUG(logger, boost::str(boost::format("HistSlope Testing for OPEN: slowAngle: %.2f >= %.2f ?, fastAngle: %.2f >= %.2f ?")
				% slowAngle % m_slowAngleThreshold % fastAngle % m_fastAngleThreshold));
			if(DoubleGreaterEqual(slowAngle, m_slowAngleThreshold)
				&& DoubleGreaterEqual(fastAngle, m_fastAngleThreshold))
			{
				return true;
			}
		}
	}
	return false;
}

bool CHistSlopeCloseTrigger::OnTest( const double vals[], int size )
{
	if(size == 2 && m_direction > entity::NET)
	{
		double fastAngle = fabs(vals[0]);
		double slowAngle = fabs(vals[1]);

		if(m_offset == entity::CLOSE)
		{
			double fa = vals[0];
			double sa = vals[1];

			LOG_DEBUG(logger, boost::str(boost::format("HistSlope Testing for CLOSE: slowAngle: %.2f, fastAngle: %.2f")
				% sa % fa));

			bool isSlowAngleGood = m_direction == entity::LONG ? sa > 0 : sa < 0;

			if(isSlowAngleGood)
			{
				if(fa * sa < 0 &&	// fast and slow have different slope direction
					DoubleGreaterEqual(fastAngle, m_fastAngleThreshold))// fast Angle different than slow Angle, AND > 45
				{
					LOG_DEBUG(logger, boost::str(boost::format("Fast Angle has become against Slow Angle and greater than %.2f") % m_fastAngleThreshold));
					return true;
				}
			}
			else
			{
				LOG_DEBUG(logger, boost::str(boost::format("Slow Angle has reversed. (sa:%.2f)") % sa));
				return true;
			}
			
		}
	}
	return false;
}
