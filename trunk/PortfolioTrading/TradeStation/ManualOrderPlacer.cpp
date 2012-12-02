#include "StdAfx.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"
#include "charsetconvert.h"

#include <boost/format.hpp>

#define DEFAULT_RETRY_TIMES 2

const char* STATE_TEXT[] = {"Order SENT", 
	"CANCELING Order", "Order COMPLETE", "Place FAILED"};
const char* EVENT_TEXT[] = {"Order COMPLETE", "Order PENDING",
	"Order CANCEL Success", "Order CANCEL Failed", "Order SUBMIT Failed", "Order REJECTED"};

string COrderEvent::m_emptyStatusMsg = "";

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

void CManualOrderPlacer::OnEnter( ORDER_STATE state, COrderEvent* transEvent )
{
	string dbText = boost::str(boost::format("Order(%s) enter %s") 
		% m_currentOrdRef.c_str() % STATE_TEXT[state]);
	logger.Debug(dbText);
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
			trade::Order* pOrd = transEvent != NULL ? transEvent->RtnOrder() : NULL;
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

			if(transEvent != NULL)
			{
				trade::Order* pOrd = transEvent->RtnOrder();
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
			
			m_cond.notify_one();
		}
		break;
	}
}

void CStateOwner::CurrentState( COrderState* val )
{
	if(m_currentState != NULL)
	{
		ORDER_STATE beforeState = m_currentState->State();
		ORDER_STATE afterState = val->State();
		logger.Debug(boost::str(boost::format("%s -> %s") 
			% STATE_TEXT[beforeState] % STATE_TEXT[afterState]));
	}

	m_currentState = val;
}

void CPlaceOrderStateMachine::Transition( const string& orderRef, COrderEvent& event )
{
	boost::recursive_mutex::scoped_lock lock(m_mut);
	ORDER_EVENT ordEvt = event.Event();
	logger.Debug(boost::str(boost::format("Order(%s) transition event %s")
		% orderRef.c_str() % EVENT_TEXT[ordEvt]));

	OrderPlacerMapIter iter = m_orderPlacers.find(orderRef);
	if(iter != m_orderPlacers.end())
	{
		COrderState* currentState = iter->second->CurrentState();

		COrderState* pNextState = currentState->Next(event);
		if(pNextState != NULL)
		{
			pNextState->Run(iter->second, &event);
		}
		else
		{
			logger.Warning(boost::str(boost::format("Unexpected event(%s) for state(%s)")
				% EVENT_TEXT[ordEvt] % STATE_TEXT[currentState->State()]));
		}
	}
	else
	{
		logger.Warning(boost::str(boost::format("Didn't find order placer for %s")
			% orderRef));
	}
}

void CPlaceOrderStateMachine::Init()
{
	OrderStatePtr sent(new OrderSent);
	m_orderStates.insert(make_pair(sent->State(), sent));
	OrderStatePtr canceling(new Canceling);
	m_orderStates.insert(make_pair(canceling->State(), canceling));
	OrderStatePtr complete(new OrderComplete);
	m_orderStates.insert(make_pair(complete->State(), complete));
	OrderStatePtr failed(new PlaceFailed);
	m_orderStates.insert(make_pair(failed->State(), failed));
	
	sent->AddEventState(ORDER_EVENT_COMPLETE, complete.get());
	sent->AddEventState(ORDER_EVENT_PENDING, canceling.get());
	sent->AddEventState(ORDER_EVENT_SUBMIT_FAILED, failed.get());
	sent->AddEventState(ORDER_EVENT_REJECTED, failed.get());

	canceling->AddEventState(ORDER_EVENT_CANCEL_FAILED, failed.get());
	canceling->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, sent.get());
}
