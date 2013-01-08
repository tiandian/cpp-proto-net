#include "StdAfx.h"
#include "MLOrderStateMachine.h"
#include "PortfolioOrderHelper.h"
#include "SgOrderStateMachine.h"
#include "OrderProcessor2.h"
//#include "DiffStrategy.h"

#include <vector>
#include <boost/shared_ptr.hpp>

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

//void CMLOrderStateMachine::Transition( const string& orderId, COrderEvent& event )
//{
//
//}

void CMLOrderPlacer::Do()
{

}

void CMLOrderPlacer::Send()
{
	InputOrderVectorPtr vecInputOrders(new InputOrderVector);
	int ordCount = GetInputOrders(m_mlOrder.get(), vecInputOrders);

	/*bool autoTracking = m_pPortf->Strategy()->IsAutoTracking();
	bool enablePrefer = m_pPortf->Strategy()->EnablePrefer();

	if(enablePrefer)
	{

	}
	else
	{

	}*/
}

void CMLOrderPlacer::CreateSgOrderPlacer(const boost::shared_ptr<trade::InputOrder>& inputOrder)
{
	m_pOrderProcessor->CreateSingleOrderPlacer(m_mlOrder.get(), inputOrder);
}
