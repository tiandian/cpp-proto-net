#pragma once
#include "Trigger.h"

class CScalperTrigger : public CTrigger
{
public:
	CScalperTrigger(const entity::TriggerItem& triggerItem);
	~CScalperTrigger(void);

private:
	double m_threshold;
};

