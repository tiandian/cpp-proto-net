#include "StdAfx.h"
#include "MLOrderStateMachine.h"
#include "PortfolioOrderHelper.h"
#include "SgOrderStateMachine.h"
#include "OrderProcessor2.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

typedef std::vector<boost::shared_ptr<trade::InputOrder>> InputOrderVector;
typedef InputOrderVector* InputOrderVectorPtr;

CMLOrderStateMachine::CMLOrderStateMachine(void)
{
}


CMLOrderStateMachine::~CMLOrderStateMachine(void)
{
}

void CMLOrderStateMachine::Init()
{
	OrderStatePtr sent(new OrderSent);
	m_orderStates.insert(make_pair(sent->State(), sent));

	OrderStatePtr partiallyFilled(new OrderPartiallyFilled);
	m_orderStates.insert(make_pair(partiallyFilled->State(), partiallyFilled));
	OrderStatePtr complete(new OrderComplete);
	m_orderStates.insert(make_pair(complete->State(), complete));

	OrderStatePtr partiallyCanceled(new OrderPartiallyCanceled);
	m_orderStates.insert(make_pair(partiallyCanceled->State(), partiallyCanceled));
	OrderStatePtr canceled(new OrderCanceled);
	m_orderStates.insert(make_pair(canceled->State(), canceled));
	
	OrderStatePtr partiallyPlaceFailed(new OrderPartiallyFailed);
	m_orderStates.insert(make_pair(partiallyPlaceFailed->State(), partiallyPlaceFailed));
	OrderStatePtr failed(new OrderPlaceFailed);
	m_orderStates.insert(make_pair(failed->State(), failed));

	OrderStatePtr warn(new OrderWarningState);
	m_orderStates.insert(make_pair(warn->State(), warn));

	sent->AddEventState(ORDER_EVENT_COMPLETE, partiallyFilled.get());
	sent->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, partiallyCanceled.get());
	sent->AddEventState(ORDER_EVENT_SUBMIT_FAILED, partiallyPlaceFailed.get());
	
	partiallyFilled->AddEventState(ORDER_EVENT_COMPLETE, complete.get());
	partiallyFilled->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, warn.get());
	partiallyFilled->AddEventState(ORDER_EVENT_SUBMIT_FAILED, warn.get());

	partiallyCanceled->AddEventState(ORDER_EVENT_COMPLETE, warn.get());
	partiallyCanceled->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, canceled.get());
	partiallyCanceled->AddEventState(ORDER_EVENT_SUBMIT_FAILED, failed.get());

	partiallyPlaceFailed->AddEventState(ORDER_EVENT_COMPLETE, warn.get());
	partiallyPlaceFailed->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, failed.get());
	partiallyPlaceFailed->AddEventState(ORDER_EVENT_SUBMIT_FAILED, failed.get());
	
}

void CMLOrderStateMachine::Transition( const string& orderId, COrderEvent& event )
{
	COrderStateMachine::Transition(orderId, event);
}

void CMLOrderPlacer::Do()
{

}

void CMLOrderPlacer::Send()
{
	InputOrderVectorPtr vecInputOrders(new InputOrderVector);
	int ordCount = GetInputOrders(m_mlOrder.get(), vecInputOrders);

	bool autoTracking = m_pPortf->AutoTracking();
	bool enablePrefer = m_pPortf->EnablePrefer();

	for(InputOrderVector::iterator iter = vecInputOrders->begin();
		iter != vecInputOrders->end(); ++iter)
	{
		m_sgOrderPlacers.push_back(CreateSgOrderPlacer(*iter, autoTracking ? 2 : 0));
	}

	if(enablePrefer)
	{
		// Only start the first one
		(m_sgOrderPlacers.front())->Do();
	}
	else
	{
		BOOST_FOREACH(const OrderPlacerPtr& placer, m_sgOrderPlacers)
		{
			placer->Do();
		}
	}
}

OrderPlacerPtr CMLOrderPlacer::CreateSgOrderPlacer(const boost::shared_ptr<trade::InputOrder>& inputOrder, int retryTimes)
{
	return m_pOrderProcessor->CreateSingleOrderPlacer(m_mlOrder.get(), inputOrder, retryTimes);
}
