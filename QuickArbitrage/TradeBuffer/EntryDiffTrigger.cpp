#include "StdAfx.h"
#include "EntryDiffTrigger.h"
#include "Portfolio.h"


CEntryDiffTrigger::CEntryDiffTrigger(CPortfolio* parentPortfolio):
	m_Portfolio(parentPortfolio)
{
}


CEntryDiffTrigger::~CEntryDiffTrigger(void)
{
}

bool CEntryDiffTrigger::OnConditionCheck( CTP::Quote* quote )
{
	return false;
}

void CEntryDiffTrigger::OnTriggered()
{
	InvokeHandler(m_Portfolio->GetID());
}
