#pragma once
#include "trigger.h"

class CPortfolio;

class CEntryDiffTrigger :
	public CTrigger
{
public:
	CEntryDiffTrigger(CPortfolio* parentPortfolio);
	~CEntryDiffTrigger(void);

protected:

	virtual bool OnConditionCheck(CTP::Quote* quote);
	virtual void OnTriggered();

private:
	CPortfolio* m_Portfolio;
};

