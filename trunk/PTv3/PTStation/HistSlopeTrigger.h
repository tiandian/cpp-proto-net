#pragma once
#include "Trigger.h"

#define HistSlopeTriggerName "HistSlopeTrigger"
#define HistSlopeTrailingStopTriggerName "HistSlopeTrailingStopTrigger"

class CHistSlopeTrigger : public CTrigger
{
public:
	CHistSlopeTrigger(const entity::TriggerItem& triggerItem);
	virtual ~CHistSlopeTrigger(void);

	void Apply( const entity::TriggerItem& triggerItem );

	// Input value should be latest price
	virtual bool OnTest(const double vals[], int size){ return false; }

	entity::PosiOffsetFlag Offset(){ return m_offset; }

protected:
	double m_fastAngleThreshold;
	double m_slowAngleThreshold;
	entity::PosiOffsetFlag m_offset;
};

class CHistSlopeOpenTrigger : public CHistSlopeTrigger
{
public:
	CHistSlopeOpenTrigger(const entity::TriggerItem& triggerItem) : CHistSlopeTrigger(triggerItem)
	{}
	~CHistSlopeOpenTrigger(){}

	virtual bool OnTest(const double vals[], int size);
};

class CHistSlopeCloseTrigger : public CHistSlopeTrigger
{
public:
	CHistSlopeCloseTrigger(const entity::TriggerItem& triggerItem) 
		: CHistSlopeTrigger(triggerItem)
		, m_direction(entity::NET)
	{}
	~CHistSlopeCloseTrigger(void){}

	virtual bool OnTest(const double vals[], int size);

	void SetDirection(entity::PosiDirectionType direction){ m_direction = direction; }
	entity::PosiDirectionType GetDirection(){ return m_direction; }

private:
	entity::PosiDirectionType m_direction;
};


