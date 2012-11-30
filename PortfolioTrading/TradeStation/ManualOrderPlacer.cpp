#include "StdAfx.h"
#include "ManualOrderPlacer.h"

#define DEFAULT_RETRY_TIMES 2

CManualOrderPlacer::CManualOrderPlacer(CPlaceOrderStateMachine* pStateMachine):
m_pStateMachine(pStateMachine),
m_succ(false),
m_retryTimes(DEFAULT_RETRY_TIMES)
{
}


CManualOrderPlacer::~CManualOrderPlacer(void)
{
}

bool CManualOrderPlacer::Do()
{
	boost::unique_lock<boost::mutex> lock(m_mut);

	PrepareInputOrder();

	COrderState* sentState = m_pStateMachine->GetState(ORDER_STATE_SENT);
	sentState->Run(this, NULL);

	m_cond.wait(lock);

	return m_succ;
}

void CManualOrderPlacer::PrepareInputOrder()
{
	m_inputOrder = boost::shared_ptr<trade::InputOrder>(m_buildFunc());
	m_currentOrdRef = m_inputOrder->orderref();
	m_pStateMachine->AddPlacer(m_currentOrdRef, this);
}

void CManualOrderPlacer::OnEnter( ORDER_STATE state, trade::Order* pOrd )
{
	switch(state)
	{
	case ORDER_STATE_SENT:
		{
			if(m_retryTimes > 0)
			{
				if(m_retryTimes < DEFAULT_RETRY_TIMES)
				{
					m_pStateMachine->RemovePlacer(m_currentOrdRef);
					PrepareInputOrder();
				}
				m_submitFunc(m_inputOrder.get());
			}
			else
			{
				boost::mutex::scoped_lock lock(m_mut);
				m_succ = false;
				m_errorMsg = "Retry times is used up";
				m_cond.notify_one();
			}
		}
		break;
	case ORDER_STATE_CANCELING:
		{
			if(pOrd != NULL)
			{
				const std::string& ordRef = pOrd->orderref();
				const std::string& exchId = pOrd->exchangeid();
				const std::string& ordSysId = pOrd->ordersysid(); 
				const std::string& userId = pOrd->userid();
				const std::string& symbol = pOrd->instrumentid();
				m_cancelFunc(ordRef, exchId, ordSysId, userId, symbol);
			}
		}
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

