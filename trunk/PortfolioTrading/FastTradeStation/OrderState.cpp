#include "OrderState.h"
#include "globalmembers.h"

#include <boost/format.hpp>

const char* ORDER_STATE_TEXT[] = {
	"Order Sending",
	"Order SENT", 
	"Order PENDING", 
	"Order COMPLETE", "Partially Filled",
	"Order CANCELED", "Partially Canceled",
	"Place FAILED", "Partialy Failed", };
const char* ORDER_EVENT_TEXT[] = {"Order COMPLETED", "Order is PENDING",
	"Order CANCEL Success", "Order CANCEL Failed", 
	"Order SUBMIT Success", "Order SUBMIT Failed", 
	"Order REJECTED", "Order PARTIALLY Filled", 
	"Order pending Time up", "Next quote coming"};

string op2::COrderEvent::m_defaultStatusMsg = "";

bool op2::CStateOwner::CurrentState( COrderState* val )
{
	if(m_currentState != NULL)
	{
		ORDER_STATE beforeState = m_currentState->State();
		ORDER_STATE afterState = val->State();
		logger.Debug(boost::str(boost::format("%s -> %s") 
			% ORDER_STATE_TEXT[beforeState] % ORDER_STATE_TEXT[afterState]));
	}

	bool changed = m_currentState != val;

	m_currentState = val;

	return changed;
}

const char* op2::CStateOwner::PrintState( ORDER_STATE state )
{
	return ORDER_STATE_TEXT[state];
}

const char* op2::CStateOwner::PrintEvent( ORDER_EVENT evt )
{
	return ORDER_EVENT_TEXT[evt];
}
