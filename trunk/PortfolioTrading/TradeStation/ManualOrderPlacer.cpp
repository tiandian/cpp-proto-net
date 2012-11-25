#include "StdAfx.h"
#include "ManualOrderPlacer.h"


CManualOrderPlacer::CManualOrderPlacer(CPlaceOrderStateMachine* pStateMachine):
m_pStateMachine(pStateMachine)
{
}


CManualOrderPlacer::~CManualOrderPlacer(void)
{
}

bool CManualOrderPlacer::Do()
{
	m_inputOrder = boost::shared_ptr<trade::InputOrder>(m_buildFunc());
	m_currentOrdRef = m_inputOrder->orderref();
	m_pStateMachine->AddPlacer(m_currentOrdRef, this);

	COrderState* sentState = m_pStateMachine->GetState(ORDER_STATE_SENT);
	sentState->Run(this);
}

void CManualOrderPlacer::OnEnter( ORDER_STATE state )
{
	switch(state)
	{
	case ORDER_STATE_SENT:
		m_submitFunc(m_inputOrder.get());
		break;
	}
}
