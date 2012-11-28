#include "StdAfx.h"
#include "ManualOrderPlacer.h"


CManualOrderPlacer::CManualOrderPlacer(CPlaceOrderStateMachine* pStateMachine):
m_pStateMachine(pStateMachine),
m_succ(false)
{
}


CManualOrderPlacer::~CManualOrderPlacer(void)
{
}

bool CManualOrderPlacer::Do()
{
	boost::unique_lock<boost::mutex> lock(m_mut);

	m_inputOrder = boost::shared_ptr<trade::InputOrder>(m_buildFunc());
	m_currentOrdRef = m_inputOrder->orderref();
	m_pStateMachine->AddPlacer(m_currentOrdRef, this);

	COrderState* sentState = m_pStateMachine->GetState(ORDER_STATE_SENT);
	sentState->Run(this);

	m_cond.wait(lock);

	return m_succ;
}

void CManualOrderPlacer::OnEnter( ORDER_STATE state )
{
	switch(state)
	{
	case ORDER_STATE_SENT:
		m_submitFunc(m_inputOrder.get());
		break;
	case ORDER_STATE_CANCELING:
		break;
	case ORDER_STATE_COMPLETE:
		{
			boost::mutex::scoped_lock lock(m_mut);
			m_succ = true;
			m_cond.notify_one();
		}
		break;
	case ORDER_STATE_PLACE_FAILED:
		{
			boost::mutex::scoped_lock lock(m_mut);
			m_succ = false;
			m_errorMsg = "Order not completed";
			m_cond.notify_one();
		}
		break;

	}
}
