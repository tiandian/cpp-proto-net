#include "StdAfx.h"
#include "SgOrderStateMachine.h"

using namespace op2;

CSgOrderStateMachine::CSgOrderStateMachine(void)
{
}


CSgOrderStateMachine::~CSgOrderStateMachine(void)
{
}

void CSgOrderStateMachine::Init()
{
	OrderStatePtr sent(new OrderSent);
	m_orderStates.insert(make_pair(sent->State(), sent));
	OrderStatePtr pending(new OrderPending);
	m_orderStates.insert(make_pair(pending->State(), pending));
	OrderStatePtr complete(new OrderComplete);
	m_orderStates.insert(make_pair(complete->State(), complete));
	OrderStatePtr failed(new OrderPlaceFailed);
	m_orderStates.insert(make_pair(failed->State(), failed));

	sent->AddEventState(ORDER_EVENT_COMPLETE, complete.get());
	sent->AddEventState(ORDER_EVENT_PENDING, pending.get());
	sent->AddEventState(ORDER_EVENT_SUBMIT_FAILED, failed.get());
	sent->AddEventState(ORDER_EVENT_REJECTED, failed.get());

	pending->AddEventState(ORDER_EVENT_CANCEL_FAILED, failed.get());
	pending->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, sent.get());
	pending->AddEventState(ORDER_EVENT_PENDING, pending.get());
	pending->AddEventState(ORDER_EVENT_COMPLETE, complete.get());
}

void CSgOrderStateMachine::Transition( const string& orderId, COrderEvent& event )
{
	COrderStateMachine::Transition(orderId, event);
}


void CSgOrderPlacer::Do()
{
	COrderState* sentState = m_pStateMachine->GetState(ORDER_STATE_SENT);
	sentState->Run(this, NULL);
}
