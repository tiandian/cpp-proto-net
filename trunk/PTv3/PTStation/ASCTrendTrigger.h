#pragma once

#include "Trigger.h"

class CASCTrendTrigger : public CTrigger
{
public:
	CASCTrendTrigger(const entity::TriggerItem& triggerItem);
	~CASCTrendTrigger(void);

	void Enable(bool isEnabled){ CTrigger::Enable(isEnabled); }

	void Enable(double cost, entity::PosiDirectionType direction);

	void Apply( const entity::TriggerItem& triggerItem ){}

	bool OnTest(const double vals[], int size);

private:

	double m_priceSummit;
	entity::PosiDirectionType m_direction;
};

