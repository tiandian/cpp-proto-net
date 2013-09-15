#pragma once
#include "Trigger.h"

#define HistSlopeTriggerName "HistSlopeTrigger"
#define HistSlopeTrailingStopTriggerName "HistSlopeTrailingStopTrigger"

class CHistSlopeTrigger : public CTrigger
{
public:
	CHistSlopeTrigger(const entity::TriggerItem& triggerItem);
	~CHistSlopeTrigger(void);

	void Apply( const entity::TriggerItem& triggerItem );

	// Input value should be latest price
	bool OnTest(const double vals[], int size);

	entity::PosiOffsetFlag Offset(){ return m_offset; }

private:
	double m_fastAngleThreshold;
	double m_slowAngleThreshold;
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

