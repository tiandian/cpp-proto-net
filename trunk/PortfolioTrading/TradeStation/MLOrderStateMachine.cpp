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
