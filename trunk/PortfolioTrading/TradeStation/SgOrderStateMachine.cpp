#include "StdAfx.h"
#include "SgOrderStateMachine.h"
#include "globalmembers.h"
#include "OrderProcessor2.h"
#include "charsetconvert.h"

#include <boost/format.hpp>

using namespace op2;

trade::Order* GetOrderBySymbol(trade::MultiLegOrder* mlOrder, const string& symbol)
{
	trade::Order* pOrdFound = NULL;
	int count = mlOrder->legs_size();
	google::protobuf::RepeatedPtrField<trade::Order>* legs = mlOrder->mutable_legs();
	for(int i = 0; i < count; ++i)
	{
		trade::Order* pOrd = legs->Mutable(i);
		if(pOrd->instrumentid() == symbol)
		{
			// find it
			pOrdFound = pOrd;
			break;
		}
	}
	return pOrdFound;
}

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

void CSgOrderStateMachine::Transition( const string& orderId/*orderRef*/, COrderEvent& event )
{
	COrderStateMachine::Transition(orderId, event);
}


void CSgOrderPlacer::Do()
{
	COrderState* sentState = m_pStateMachine->GetState(ORDER_STATE_SENT);
	sentState->Run(this, NULL);
}

bool CSgOrderPlacer::OnEnter( ORDER_STATE state, COrderEvent* transEvent )
{
	string dbText = boost::str(boost::format("Order(%s - %s) enter %s") 
		% ParentOrderId() % Symbol() % PrintState(state));
	logger.Debug(dbText);
	bool isTerminal = false;

	CSgOrderEvent* pSgOrderEvent = dynamic_cast<CSgOrderEvent*>(transEvent);
	_ASSERT(pSgOrderEvent != NULL);
	if(pSgOrderEvent == NULL)
		return isTerminal;

	switch(state)
	{
	case ORDER_STATE_SENT:
		{
			if(m_submitTimes <= m_maxRetryTimes)
			{
				if(m_submitTimes > 0)
					m_pStateMachine->RemovePlacer(Id());

				// lock and generate order ref
				int iOrdRef = m_pOrderProcessor->LockForSubmit(m_currentOrdRef);
				if(iOrdRef < 0) // Cannot correctly generate order ref, something wrong
				{
					m_succ = false;
					m_errorMsg = "未能生成委托引用(OrderRef)";
					isTerminal = true;
				}
				else
				{
					m_pInputOrder->set_orderref(m_currentOrdRef);
					m_pStateMachine->AddPlacer(OrderPlacerPtr(this));

					string submitInfo = boost::str(boost::format("Submit Order(%s - %s) [No. %d time(s)]")
						% ParentOrderId() % Symbol() % m_submitTimes);
					bool succ = m_pOrderProcessor->SubmitAndUnlock(m_pInputOrder.get());
					++m_submitTimes;
				}
			}
			else
			{
				m_succ = false;
				m_errorMsg = boost::str(boost::format("%d次追单后仍未成交！") % m_maxRetryTimes);
				logger.Info(boost::str(boost::format("Submit order (%s - %s ) -> Retry times is used up")
					% ParentOrderId() % Symbol()));
				isTerminal = true;

				m_pOrderProcessor->RaiseMLOrderPlacerEvent(ParentOrderId(), LegCanceledEvent());
			}
		}
		break;
	case ORDER_STATE_PENDING:
		{
			trade::Order* pOrd = pSgOrderEvent->RtnOrder();
			if(pOrd != NULL)
			{
				OnOrderUpdate(pOrd);

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
			OnOrderUpdate(pSgOrderEvent->RtnOrder());
			m_succ = true;
			isTerminal = true;
			m_pOrderProcessor->RaiseMLOrderPlacerEvent(ParentOrderId(), LegCompletedEvent());
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
					const string& errorMsg = pSgOrderEvent->StatusMsg();
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
			isTerminal = true;

			m_pOrderProcessor->RaiseMLOrderPlacerEvent(ParentOrderId(), LegRejectedEvent());
		}
		break;
	
	default:
		logger.Warning(boost::str(boost::format("Entering Single order UNHANDLED state %s")
			% PrintState(state)));
	}

	return isTerminal;
}

void CSgOrderPlacer::OnOrderUpdate( trade::Order* pOrd )
{
	if(pOrd != NULL)
	{
		trade::Order* pLegOrder = GetOrderBySymbol(m_pMultiLegOrder, pOrd->instrumentid());

		string ordStatusMsg;
		GB2312ToUTF_8(ordStatusMsg, pOrd->statusmsg().c_str());
		pLegOrder->set_statusmsg(ordStatusMsg);

		m_pOrderProcessor->PublishOrderUpdate(m_pMultiLegOrder->portfolioid(), m_pMultiLegOrder->orderid(), pLegOrder);
	}
}
