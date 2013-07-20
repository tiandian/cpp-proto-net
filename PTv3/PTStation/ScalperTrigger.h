#pragma once
#include "Trigger.h"
#include "DoubleCompare.h"

class CScalperTrigger : public CTrigger
{
public:
	CScalperTrigger(const entity::TriggerItem& triggerItem);
	~CScalperTrigger(void);

	void Apply(const entity::TriggerItem& triggerItem);

	bool OnTest(double val)
	{ 
		return DoubleGreaterEqual(val, m_threshold); 
	}

private:
	double m_threshold;
};

