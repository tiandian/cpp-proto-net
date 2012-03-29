#include "StdAfx.h"
#include "Trigger.h"


CTrigger::CTrigger(void):
	m_isTriggered(false),
	m_isActive(false),
	m_handler(NULL)
{
}


CTrigger::~CTrigger(void)
{
}

bool CTrigger::Test( CTP::Quote* quote )
{
	if(m_isActive)
	{
		if(OnConditionCheck(quote))
		{
			OnTriggered();
			m_isTriggered = true;
			return true;
		}
	}

	return false;
}
