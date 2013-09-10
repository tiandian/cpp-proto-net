#pragma once
#include "Trigger.h"

class CHistSlopeTrigger : public CTrigger
{
public:
	CHistSlopeTrigger(const entity::TriggerItem& triggerItem);
	~CHistSlopeTrigger(void);

	void Apply( const entity::TriggerItem& triggerItem );

private:
	double m_angelThreshold;
	entity::PosiOffsetFlag m_offset;
};

class CHistSlopeTrailingStop : public CTrigger
{
public:
    CHistSlopeTrailingStop()
        :CTrigger("TrailingStop"){}
    ~CHistSlopeTrailingStop(){}

    void Enable(double cost, LongShort direction)
    {
        m_lastHigh = cost;
        m_effectiveStop = CalcOffset(cost, -m_backValue);
        m_direction = direction;
        CTrigger::Enable(true);
    }

    void Apply()
    {
        // m_backValue = 
    }

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
            return true;
        }

        return false; 
    }

private:

    double CalcOffset(double base, double offset)
    {
        return m_direction == LS_LONG ? 
            base + offset : base - offset;
    }

    bool Compare(double src, double dest)
    {
        return m_direction == LS_LONG ?
            src > dest : src < dest;
    }

    double m_backValue;
    double m_effectiveStop;
    double m_lastHigh;

    LongShort m_direction;
};

