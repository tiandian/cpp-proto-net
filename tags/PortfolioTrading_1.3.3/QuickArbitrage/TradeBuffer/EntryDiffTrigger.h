#pragma once

#include "trigger.h"

#include <vector>

class CPortfolio;

class CEntryDiffTrigger :
	public CTrigger
{
public:
	CEntryDiffTrigger(CPortfolio* parentPortfolio);
	~CEntryDiffTrigger(void);

protected:

	virtual const char* GetName() { return "Diff Entry"; }
	virtual void SetParameters(TriggerParamMap& paramMap);
	virtual void GetParameters(TriggerParamMap& paramMap);

	virtual bool OnConditionCheck(CTP::Quote* quote);
	virtual void OnTriggered();

private:

	CPortfolio* m_Portfolio;
	double m_thresholdDiff;
	std::vector<double> m_legValues;
};

