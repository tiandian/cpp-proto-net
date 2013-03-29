#pragma once

#include <string>
#include <map>

using namespace std;

namespace op2
{

enum ORDER_EVENT
{
	ORDER_EVENT_COMPLETE,
	ORDER_EVENT_PENDING,
	ORDER_EVENT_CANCEL_SUCCESS,
	ORDER_EVENT_CANCEL_FAILED,
	ORDER_EVENT_SUBMIT_SUCCESS,
	ORDER_EVENT_SUBMIT_FAILED,
	ORDER_EVENT_REJECTED,
	ORDER_EVENT_PARTIALLY_FILLED,
	ORDER_EVENT_PENDING_TIMEUP,
	ORDER_EVENT_NEXT_QUOTE_IN
};

class COrderEvent
{
public:
	COrderEvent(ORDER_EVENT evt):m_event(evt){}
	virtual ~COrderEvent(){}

	ORDER_EVENT Event(){ return m_event; }

	virtual const string& StatusMsg(){ return m_defaultStatusMsg; }

protected:
	ORDER_EVENT m_event;

private:
	static string m_defaultStatusMsg;
};

class COrderState;

enum ORDER_STATE 
{
	ORDER_STATE_SENDING,
	ORDER_STATE_SENT,
	ORDER_STATE_PENDING,
	ORDER_STATE_COMPLETE,
	ORDER_STATE_PARTIALLY_FILLED,
	ORDER_STATE_CANCELED,
	ORDER_STATE_PARTIALLY_CANCELED,
	ORDER_STATE_PLACE_FAILED,
	ORDER_STATE_PARTIALLY_FAILED,
	ORDER_STATE_WARNING,
	ORDER_STATE_CANCELING
};

class CStateOwner
{
public:
	CStateOwner():m_currentState(NULL){}
	virtual ~CStateOwner(){}

	COrderState* CurrentState() const { return m_currentState; }
	bool CurrentState(COrderState* val);

	virtual bool OnEnter(ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState) { return false; };

	static const char* PrintState(ORDER_STATE state);
	static const char* PrintEvent(ORDER_EVENT evt);

protected:
	COrderState* m_currentState;

};

class COrderState
{
protected:
	COrderState(ORDER_STATE state):m_state(state)
	{}

public:
	virtual ~COrderState(){}

	ORDER_STATE State(){ return m_state; }

	void AddEventState(ORDER_EVENT event, COrderState* pState)
	{
		eventStateMap.insert(make_pair(event, pState));
	}

	// returning boolean stands for termination of state transition
	// true means state won't change any more, it's terminated
	// false means it would keep changing.
	virtual bool Run(CStateOwner* stateOwner, COrderEvent* transEvent)
	{
		COrderState* pLastSate = stateOwner->CurrentState();
		ORDER_STATE lastStateEum = pLastSate != NULL ? pLastSate->State() : (ORDER_STATE)-1;

		if(stateOwner->CurrentState(this))
			return stateOwner->OnEnter(m_state, transEvent, lastStateEum);

		return false;
	}

	virtual COrderState* Next(COrderEvent* evt)
	{ 
		map<ORDER_EVENT, COrderState*>::iterator iter = eventStateMap.find(evt->Event());
		if(iter != eventStateMap.end())
		{
			return iter->second;
		}

		return NULL;
	}

	bool IsEnd() { return eventStateMap.empty(); }

protected:
	ORDER_STATE m_state;

	map<ORDER_EVENT, COrderState*> eventStateMap;
};

class OrderSending : public COrderState
{
public:
	OrderSending():COrderState(ORDER_STATE_SENDING){}
};

class OrderSent : public COrderState
{
public:
	OrderSent():COrderState(ORDER_STATE_SENT){}
};

class OrderPending : public COrderState
{
public:
	OrderPending():COrderState(ORDER_STATE_PENDING){}
};

class OrderComplete : public COrderState
{
public:
	OrderComplete():COrderState(ORDER_STATE_COMPLETE){}
};

class OrderPlaceFailed : public COrderState
{
public:
	OrderPlaceFailed():COrderState(ORDER_STATE_PLACE_FAILED){}
};

class OrderPartiallyFilled : public COrderState
{
public:
	OrderPartiallyFilled():COrderState(ORDER_STATE_PARTIALLY_FILLED){}
};

class OrderCanceled : public COrderState
{
public:
	OrderCanceled():COrderState(ORDER_STATE_CANCELED){}
};

class OrderPartiallyCanceled : public COrderState
{
public:
	OrderPartiallyCanceled():COrderState(ORDER_STATE_PARTIALLY_CANCELED){}
};

class OrderPartiallyFailed : public COrderState
{
public:
	OrderPartiallyFailed():COrderState(ORDER_STATE_PARTIALLY_FAILED){}
};

class OrderWarningState : public COrderState
{
public:
	OrderWarningState():COrderState(ORDER_STATE_WARNING){}
};

class OrderCanceling : public COrderState
{
public:
	OrderCanceling():COrderState(ORDER_STATE_CANCELING){}
};

};