#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "PortfolioOrderHelper.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace std;

enum ORDER_EVENT
{
	ORDER_EVENT_COMPLETE,
	ORDER_EVENT_PENDING,
	ORDER_EVENT_CANCEL_SUCCESS,
	ORDER_EVENT_CANCEL_FAILED,
	ORDER_EVENT_SUBMIT_FAILED,
	ORDER_EVENT_REJECTED
};

class COrderEvent
{
public:
	COrderEvent(ORDER_EVENT evt, trade::Order* rtnOrder):m_event(evt), m_rtnOrder(rtnOrder){}
	virtual ~COrderEvent(){}

	ORDER_EVENT Event(){ return m_event; }
	trade::Order* RtnOrder(){ return m_rtnOrder; }

	virtual const string& StatusMsg(){ return m_rtnOrder != NULL ? m_rtnOrder->statusmsg() : m_emptyStatusMsg; }

protected:
	ORDER_EVENT m_event;
	trade::Order* m_rtnOrder;

private:
	static string m_emptyStatusMsg;
};

class CompleteEvent : public COrderEvent
{
public:
	CompleteEvent(trade::Order* rtnOrder):COrderEvent(ORDER_EVENT_COMPLETE, rtnOrder){}
};

class RejectEvent: public COrderEvent
{
public:
	RejectEvent(trade::Order* rtnOrder) : COrderEvent(ORDER_EVENT_REJECTED, rtnOrder){}
};

class PendingEvent : public COrderEvent
{
public:
	PendingEvent(trade::Order* rtnOrder):COrderEvent(ORDER_EVENT_PENDING, rtnOrder){}
};

class CancelSuccessEvent : public COrderEvent
{
public:
	CancelSuccessEvent(trade::Order* rtnOrder):COrderEvent(ORDER_EVENT_CANCEL_SUCCESS, rtnOrder){}
};

class CancelFailedEvent : public COrderEvent
{
public:
	CancelFailedEvent(trade::Order* rtnOrder):COrderEvent(ORDER_EVENT_CANCEL_FAILED, rtnOrder){}
};

class SubmitFailedEvent : public COrderEvent
{
public:
	SubmitFailedEvent(const string& errMsg):
	  COrderEvent(ORDER_EVENT_SUBMIT_FAILED, NULL),
	  m_errMsg(errMsg){}

	virtual const string& StatusMsg(){ return m_errMsg; }
private:
	string m_errMsg;
};

class COrderState;

enum ORDER_STATE 
{
	ORDER_STATE_SENT,
	ORDER_STATE_CANCELING,
	ORDER_STATE_COMPLETE,
	ORDER_STATE_PLACE_FAILED
};

class CStateOwner
{
public:
	CStateOwner():m_currentState(NULL){}
	virtual ~CStateOwner(){}

	COrderState* CurrentState() const { return m_currentState; }
	void CurrentState(COrderState* val);

	virtual void OnEnter(ORDER_STATE state, COrderEvent* transEvent) {};

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

	virtual void Run(CStateOwner* stateOwner, COrderEvent* transEvent)
	{
		stateOwner->CurrentState(this);
		stateOwner->OnEnter(m_state, transEvent);
	}

	virtual COrderState* Next(COrderEvent& evt)
	{ 
		map<ORDER_EVENT, COrderState*>::iterator iter = eventStateMap.find(evt.Event());
		if(iter != eventStateMap.end())
		{
			return iter->second;
		}

		return NULL;
	}

protected:
	ORDER_STATE m_state;

	map<ORDER_EVENT, COrderState*> eventStateMap;
};

class OrderSent : public COrderState
{
public:
	OrderSent():COrderState(ORDER_STATE_SENT){}
};

class Canceling : public COrderState
{
public:
	Canceling():COrderState(ORDER_STATE_CANCELING){}
};

class OrderComplete : public COrderState
{
public:
	OrderComplete():COrderState(ORDER_STATE_COMPLETE){}
};

class PlaceFailed : public COrderState
{
public:
	PlaceFailed():COrderState(ORDER_STATE_PLACE_FAILED){}
};

typedef boost::shared_ptr<COrderState> OrderStatePtr;
typedef map<ORDER_STATE, OrderStatePtr> OrderStateMap;
typedef OrderStateMap::iterator OrderStateMapIter;

typedef boost::function<trade::InputOrder*(/* const string&, trade::TradeDirectionType, trade::OffsetFlagType, PlaceOrderContext* */)> BuildOrderFunc;
typedef boost::function<void(trade::InputOrder*)> SubmitOrderFunc;
typedef boost::function<void(const string&, const string& exchId, const string& ordSysId, const string& userId, const string& symbol)> CancelOrderFunc;

class CPlaceOrderStateMachine;

class CManualOrderPlacer : public CStateOwner
{
public:
	CManualOrderPlacer(CPlaceOrderStateMachine* pStateMachine);
	~CManualOrderPlacer(void);

	void SetBuildOrderFunc(BuildOrderFunc buildFunc){ m_buildFunc = buildFunc; }
	void SetSubmitOrderFunc(SubmitOrderFunc submitFunc){ m_submitFunc = submitFunc; }
	void SetCancelOrderFunc(CancelOrderFunc cancelFunc){ m_cancelFunc = cancelFunc; }

	bool Do();
	const string& GetError(){ return m_errorMsg; }
	void SetError(const string& errorMsg){ m_errorMsg = errorMsg; }
	const string& CurrentOrderRef(){ return m_currentOrdRef; }

	void OnEnter(ORDER_STATE state, COrderEvent* transEvent);

private:

	void PrepareInputOrder();

	BuildOrderFunc m_buildFunc;
	SubmitOrderFunc m_submitFunc;
	CancelOrderFunc m_cancelFunc;

	bool m_succ;
	string m_errorMsg;
	string m_currentOrdRef;
	CPlaceOrderStateMachine* m_pStateMachine;
	boost::shared_ptr<trade::InputOrder> m_inputOrder;

	boost::condition_variable m_cond;
	boost::mutex m_mut;
	int m_retryTimes;
};

typedef boost::shared_ptr<CManualOrderPlacer> ManualOrderPlacerPtr;
typedef map<string, CManualOrderPlacer*> OrderPlacerMap;
typedef OrderPlacerMap::iterator OrderPlacerMapIter;

class CPlaceOrderStateMachine
{
public:
	CPlaceOrderStateMachine()
	{
		Init();
	}
	~CPlaceOrderStateMachine(){}

	void Init();

	ManualOrderPlacerPtr CreatePlacer()
	{ 
		ManualOrderPlacerPtr orderPlacer(new CManualOrderPlacer(this));
		return orderPlacer;
	}

	void AddPlacer(const string& orderRef, CManualOrderPlacer* orderPlacer)
	{
		boost::recursive_mutex::scoped_lock lock(m_mut);
		m_orderPlacers.insert(make_pair(orderRef, orderPlacer));
	}

	void RemovePlacer(const string& orderRef)
	{
		boost::recursive_mutex::scoped_lock lock(m_mut);
		m_orderPlacers.erase(orderRef);
	}

	void Transition(const string& orderRef, COrderEvent& event);

	COrderState* GetState(ORDER_STATE stateVal)
	{
		OrderStateMapIter iter = m_orderStates.find(stateVal);
		if(iter != m_orderStates.end())
		{
			return (iter->second).get();
		}

		return NULL;
	}

private:

	OrderStateMap m_orderStates;
	OrderPlacerMap m_orderPlacers;

	boost::recursive_mutex m_mut;
};