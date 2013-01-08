#include "StdAfx.h"
#include "OrderStateMachine.h"
#include "globalmembers.h"

#include <boost/format.hpp>


extern const char* ORDER_STATE_TEXT[];
extern const char* ORDER_EVENT_TEXT[];

COrderStateMachine::COrderStateMachine(void)
{
}


COrderStateMachine::~COrderStateMachine(void)
{
}

void COrderStateMachine::Transition( const string& orderId, COrderEvent& event )
{
	boost::recursive_mutex::scoped_lock lock(m_mut);
	ORDER_EVENT ordEvt = event.Event();
	logger.Debug(boost::str(boost::format("Order(%s) transition event %s")
		% orderId.c_str() % ORDER_EVENT_TEXT[ordEvt]));

	OrderPlacerMapIter iter = m_orderPlacerMap.find(orderId);
	if(iter != m_orderPlacerMap.end())
	{
		COrderState* currentState = iter->second->CurrentState();

		COrderState* pNextState = currentState->Next(event);
		if(pNextState != NULL)
		{
			pNextState->Run((iter->second).get(), &event);
		}
		else
		{
			logger.Warning(boost::str(boost::format("Unexpected event(%s) for state(%s)")
				% ORDER_EVENT_TEXT[ordEvt] % ORDER_STATE_TEXT[currentState->State()]));
		}
	}
	else
	{
		logger.Warning(boost::str(boost::format("Didn't find order placer for %s")
			% orderId));
	}
}
