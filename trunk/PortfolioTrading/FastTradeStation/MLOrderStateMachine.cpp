#include "MLOrderStateMachine.h"
#include "PortfolioOrderHelper.h"
#include "SgOrderStateMachine.h"
#include "OrderProcessor2.h"
#include "globalmembers.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

typedef std::vector<boost::shared_ptr<trade::InputOrder> > InputOrderVector;
typedef InputOrderVector* InputOrderVectorPtr;

CMLOrderStateMachine::CMLOrderStateMachine(void)
{
	Initialize();
}


CMLOrderStateMachine::~CMLOrderStateMachine(void)
{
}

void CMLOrderStateMachine::Initialize()
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

void CMLOrderStateMachine::Transition( const string& orderId, COrderEvent* event )
{
	COrderStateMachine::Transition(orderId, event);
}

void CMLOrderPlacer::Do()
{
	m_pPortf->BeginPlaceOrder();

	InputOrderVectorPtr vecInputOrders(new InputOrderVector);
	int ordCount = GetInputOrders(m_mlOrder.get(), vecInputOrders);
	
	bool autoTracking = m_pPortf->AutoTracking();
	int retryTimes = m_pPortf->RetryTimes();
	
	for(InputOrderVector::iterator iter = vecInputOrders->begin();
		iter != vecInputOrders->end(); ++iter)
	{
		CSgOrderPlacer* pPlacer = CreateSgOrderPlacer(*iter, autoTracking ? retryTimes : 0);
		m_sgOrderPlacers.push_back(pPlacer);
	}

	// Wrap into smart pointer and let state machine manage the life cycle
	m_pStateMachine->AddPlacer(this);

	COrderState* sentState = m_pStateMachine->GetState(ORDER_STATE_SENT);
	sentState->Run(this, NULL);

}

void CMLOrderPlacer::Send()
{
	m_pOrderProcessor->PublishMultiLegOrderUpdate(m_mlOrder.get());

	m_isSequential = m_pPortf->EnablePrefer();
	if(m_isSequential)
	{
		// Only start the first one
		m_sendingIdx = 0;
		assert(m_sgOrderPlacers.size() > 0);
		(m_sgOrderPlacers.at(0))->Do();
	}
	else
	{
		BOOST_FOREACH(COrderPlacer* placer, m_sgOrderPlacers)
		{
			placer->Do();
		}
	}
}

bool CMLOrderPlacer::SendNext(COrderEvent* transEvent)
{
	if(++m_sendingIdx < (int)m_sgOrderPlacers.size())
	{
		CSgOrderPlacer*& pSgPlacer = m_sgOrderPlacers.at(m_sendingIdx);
		
		// Partically for Scalper strategy
		// ensure close order qty align to opened quantity
		if(m_pPortf->SelfClose())
		{
			LegCompletedEvent* pLegCompleteEvent = dynamic_cast<LegCompletedEvent*>(transEvent);
			if(pLegCompleteEvent != NULL)
			{
				int finished = pLegCompleteEvent->Finished();
				pSgPlacer->AdjustQuantity(finished);
			}
		}

		pSgPlacer->Do();
		return true;
	}

	return false;
}

CSgOrderPlacer* CMLOrderPlacer::CreateSgOrderPlacer(const boost::shared_ptr<trade::InputOrder>& inputOrder, int retryTimes)
{
	return m_pOrderProcessor->CreateSingleOrderPlacer(m_pPortf, m_mlOrder.get(), inputOrder, retryTimes);
}

bool CMLOrderPlacer::OnEnter( ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState )
{
	string dbText = boost::str(boost::format("MultiLeg Order(%s) enter %s") 
		% Id() % PrintState(state));
	logger.Debug(dbText);
	bool isTerminal = false;

	switch(state)
	{
	case ORDER_STATE_SENT:
		{
			Send();
		}
		break;
	case ORDER_STATE_PARTIALLY_FILLED:
		{
			if(m_isSequential)
			{
				if(SendNext(transEvent))
					break;
				//else if there is no next placer to do
				// go to next case "ORDER_STATE_COMPLETE"
				logger.Debug("There is no next placer to do. GO to Complete state");
			}
			else if (m_sgOrderPlacers.size() > 1)
				break;	// if there is only 1 order in portfolio, recognized as partially filled
						// otherwise got to complete state
		}
		//break;
	case ORDER_STATE_COMPLETE:
		{
			trade::MlOrderOffset offset = m_mlOrder->offset();
			if(offset == trade::ML_OF_OPEN || offset == trade::ML_OF_OTHER)
				m_pPortf->AddPosition(*m_mlOrder);
			else if(offset == trade::ML_OF_CLOSE)
				m_pPortf->RemovePosition(*m_mlOrder);

			isTerminal = true;
		}
		break;
	case ORDER_STATE_PARTIALLY_CANCELED:
		{
			if(m_isSequential)
			{
				isTerminal = true;
				OutputStatus("Portfolio Order Canceled");
			}
		}
		break;
	case ORDER_STATE_CANCELED:
		{
			isTerminal = true;
			OutputStatus("Portfolio Order Canceled");
		}
		break;
	case ORDER_STATE_PARTIALLY_FAILED:
		{
			if(m_isSequential)
			{
				isTerminal = true;
				OutputStatus("Portfolio order place failed");
			}
		}
		break;
	case ORDER_STATE_PLACE_FAILED:
		{
			isTerminal = true;
			OutputStatus("Portfolio order place failed");
		}
		break;
	case ORDER_STATE_WARNING:
		{
			m_mlOrder->set_haswarn(true);

			CMLegOrderEvent* pOrderEvent = dynamic_cast<CMLegOrderEvent*>(transEvent);
			assert(pOrderEvent != NULL);
			if(pOrderEvent == NULL)
				return isTerminal;

			string warnMsg;
			ORDER_EVENT evt = pOrderEvent->Event();
			switch(evt)
			{
			case ORDER_EVENT_CANCEL_SUCCESS:
				warnMsg = "Leg Order Canceled though prior order filled";
				break;
			case ORDER_EVENT_SUBMIT_FAILED:
				warnMsg = "Leg Order Failed though prior order filled";
				break;
			case ORDER_EVENT_COMPLETE:
				warnMsg = "A Leg Order gets Filled but prior order doesn't";
				break;
			default:
				logger.Warning(
					boost::str(boost::format(
					"MultiLeg Order enter WARN state due to unexpected event(%s)")
					% PrintEvent(evt)));
			}
			OutputStatus(warnMsg);
			isTerminal = true;
		}
		break;
	default:
		logger.Warning(boost::str(boost::format("Entering MultiLeg order UNHANDLED state %s")
			% PrintState(state)));
	}

	if(isTerminal)
		m_pPortf->EndPlaceOrder();

	return isTerminal;
}

void CMLOrderPlacer::OutputStatus(const string& statusMsg)
{
	m_mlOrder->set_statusmsg(statusMsg);
	m_pOrderProcessor->PublishMultiLegOrderUpdate(m_mlOrder.get());
}
