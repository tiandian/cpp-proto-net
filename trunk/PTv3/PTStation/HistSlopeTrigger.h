#pragma once
#include "Trigger.h"

class CHistSlopeTrigger : public CTrigger
{
public:
	CHistSlopeTrigger(const entity::TriggerItem& triggerItem);
	~CHistSlopeTrigger(void);

	void Apply( const entity::TriggerItem& triggerItem );

	// Input value should be latest price
	bool OnTest(double val)
	{
		double absVal = fabs(val);
		// return true if absolute value larger than or equal to threshold
		return absVal > m_angelThreshold			// explicitly larger
			|| m_angelThreshold - absVal < 0.001;	// regard as equal if diff less then 0.001
	}

private:
	double m_angelThreshold;
	entity::PosiOffsetFlag m_offset;
};

class CHistSlopeTrailingStop : public CTrigger
{
public:
    CHistSlopeTrailingStop(const entity::TriggerItem& triggerItem);
    ~CHistSlopeTrailingStop(){}

    void Enable(double cost, entity::PosiDirectionType direction)
    {
        m_lastHigh = cost;
        m_effectiveStop = CalcOffset(cost, -m_backValue);
        m_direction = direction;
        CTrigger::Enable(true);
    }

    void Apply( const entity::TriggerItem& triggerItem );

    // Input value should be latest price
    bool OnTest(double val)
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

