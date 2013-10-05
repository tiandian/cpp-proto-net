#pragma once

#include "Trigger.h"

class CTrailingStopTrigger : public CTrigger
{
public:
	CTrailingStopTrigger(const entity::TriggerItem& triggerItem);
	~CTrailingStopTrigger(){}

	void Enable(bool isEnabled){ CTrigger::Enable(isEnabled); }

	void Enable(double cost, entity::PosiDirectionType direction);

	void Apply( const entity::TriggerItem& triggerItem );

	// Input value should be latest price
	bool OnTest(double val);

private:

	double CalcOffset(double base, double offset)
	{
		return m_direction == entity::LONG ? 
			base + offset : base - offset;
	}

	bool Compare(double src, double dest)
	{
		return m_direction == entity::LONG ?
			src > dest : src < dest;
	}

	double m_backValue;
	double m_effectiveStop;
	double m_lastHigh;

	entity::PosiDirectionType m_direction;
};

