#include "StdAfx.h"
#include "SgOrderStateMachine.h"
#include "globalmembers.h"
#include "OrderProcessor2.h"
#include "charsetconvert.h"

#include <boost/format.hpp>


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

void CSgOrderPlacer::OnEnter( ORDER_STATE state, COrderEvent* transEvent )
{
	string dbText = boost::str(boost::format("Order(%s - %s) enter %s") 
		% ParentOrderId() % Symbol() % PrintState(state));
	logger.Debug(dbText);

	CSgOrderEvent* pSgOrderEvent = dynamic_cast<CSgOrderEvent*>(transEvent);
	_ASSERT(pSgOrderEvent != NULL);
	if(pSgOrderEvent == NULL)
		return;

	switch(state)
	{
	case ORDER_STATE_SENT:
		{
			if(m_submitTimes <= m_maxRetryTimes)
			{
				string submitInfo = boost::str(boost::format("Submit Order(%s - %s) [No. %d time(s)]")
					% ParentOrderId() % Symbol() % m_submitTimes);
				m_pOrderProcessor->SubmitOrderToTradeAgent(m_pInputOrder.get());
				++m_submitTimes;
			}
			else
			{
				m_succ = false;
				m_errorMsg = boost::str(boost::format("%d次追单后仍未成交！") % m_maxRetryTimes);
				logger.Info(boost::str(boost::format("Submit order (%s - %s ) -> Retry times is used up")
					% ParentOrderId() % Symbol()));
			}
		}
		break;
	case ORDER_STATE_PENDING:
		{
			trade::Order* pOrd = transEvent != NULL ? pSgOrderEvent->RtnOrder() : NULL;
			if(pOrd != NULL)
			{
				const std::string& ordRef = pOrd->orderref();
				const std::string& exchId = pOrd->exchangeid();
				const std::string& ordSysId = pOrd->ordersysid(); 
				const std::string& userId = pOrd->userid();
				const std::string& symbol = pOrd->instrumentid();
				m_pOrderProcessor->CancelOrder(ordRef, exchId, ordSysId, userId, symbol);
			}
		}
		break;
	case ORDER_STATE_COMPLETE:
		{
			m_succ = true;
		}
		break;
	case ORDER_STATE_PLACE_FAILED:
		{
			m_succ = false;

			if(pSgOrderEvent != NULL)
			{
				trade::Order* pOrd = pSgOrderEvent->RtnOrder();
				if(pOrd != NULL)
				{
					const string& status = pOrd->statusmsg();
					if(!status.empty())
					{
						GB2312ToUTF_8(m_errorMsg, status.c_str());
					}
				}
				else
				{
					const string& errorMsg = transEvent->StatusMsg();
					if(errorMsg.empty())
					{
						m_errorMsg = "Order not completed";
					}
					else
					{
						m_errorMsg = errorMsg;
					}
				}
			}
			else
			{
				m_errorMsg = "Order not completed";
			}
		}
		break;
	default:
		logger.Warning(boost::str(boost::format("Entering UNHANDLED state %s")
			% PrintState(state)));
	}
}
