#include "PortfolioOrderPlacer.h"
#include "InputOrder.h"
#include "InputOrderPlacer.h"
#include "Portfolio.h"
#include "OrderProcessor2.h"
#include "globalmembers.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>


CPortfolioOrderPlacer::CPortfolioOrderPlacer(CPortfolio* pPortf, COrderProcessor2* pOrderProc)
	: m_pPortf(pPortf)
	, m_pOrderProcessor(pOrderProc)
	, m_isWorking(false)
	, m_isSequential(false)
	, m_sendingIdx(0)
{
}

CPortfolioOrderPlacer::~CPortfolioOrderPlacer(void)
{
}

void CPortfolioOrderPlacer::SetMlOrderId( const string& mlOrdId )
{
	m_multiLegOrderTemplate->set_orderid(mlOrdId);
	m_multiLegOrderTemplate->set_openorderid(mlOrdId);

	m_multiLegOrderTemplate->set_haswarn(false);
	m_multiLegOrderTemplate->set_statusmsg("");
}

void CPortfolioOrderPlacer::Prepare()
{
	BuildTemplateOrder();

	// Create input orders from template multi-leg order
	vector<boost::shared_ptr<CInputOrder> > inputOrders;
	int inputOrdCount = GetInputOrders(&inputOrders);

	bool autoTracking = m_pPortf->AutoTracking();
	int retryTimes = m_pPortf->RetryTimes();

	for(vector<boost::shared_ptr<CInputOrder> >::iterator iter = inputOrders.begin();
		iter != inputOrders.end(); ++iter)
	{
		boost::shared_ptr<CInputOrderPlacer> pPlacer(
			CreateInputOrderPlacer(*iter, autoTracking ? retryTimes : 0));
		m_inputOrderPlacers.push_back(pPlacer);
	}
}

int CPortfolioOrderPlacer::GetInputOrders( vector<boost::shared_ptr<CInputOrder> > * genInputOrders )
{
	assert(genInputOrders != NULL);

	BOOST_FOREACH(const trade::Order& o, m_multiLegOrderTemplate->legs())
	{
		boost::shared_ptr<CInputOrder> inputOrder(new CInputOrder);

		inputOrder->set_brokerid(o.brokerid());
		inputOrder->set_investorid(o.investorid());
		inputOrder->set_symbol(o.instrumentid());
		inputOrder->set_orderref(o.orderref());
		inputOrder->set_userid(o.userid());
		inputOrder->set_orderpricetype(o.orderpricetype());
		inputOrder->set_direction(o.direction());
		inputOrder->set_combhedgeflag(o.combhedgeflag());
		inputOrder->set_comboffsetflag(o.comboffsetflag());
		inputOrder->set_limitprice(o.limitprice());
		inputOrder->set_volumetotaloriginal(o.volumetotaloriginal());
		inputOrder->set_timecondition(o.timecondition());
		inputOrder->set_gtddate(o.gtddate());
		inputOrder->set_volumecondition(	o.volumecondition());
		inputOrder->set_minvolume(o.minvolume());
		inputOrder->set_contingentcondition(	o.contingentcondition());
		inputOrder->set_stopprice(o.stopprice());
		inputOrder->set_forceclosereason(o.forceclosereason());
		inputOrder->set_isautosuspend(o.isautosuspend());
		inputOrder->set_businessunit(o.businessunit());
		inputOrder->set_requestid(o.requestid());
		inputOrder->set_userforceclose(o.userforceclose());

		if(o.preferred())	// always put preferred order at front
			genInputOrders->insert(genInputOrders->begin(), inputOrder);
		else
			genInputOrders->push_back(inputOrder);
	}

	return genInputOrders->size();
}

CInputOrderPlacer* CPortfolioOrderPlacer::CreateInputOrderPlacer(const boost::shared_ptr<CInputOrder>& inputOrder, int retryTimes)
{
	return m_pOrderProcessor->CreateInputOrderPlacer(m_pPortf, m_multiLegOrderTemplate.get(), inputOrder, retryTimes);
}

void CPortfolioOrderPlacer::Do()
{
	// Send Order immediately
	Send();

	UpdateMultiLegOrder();
}

void CPortfolioOrderPlacer::Run(trade::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize)
{
	m_isWorking = true;

	// Generate order Id
	string mlOrderId;
	m_pPortf->NewOrderId(mlOrderId);
	SetMlOrderId(mlOrderId);

	// Add to MlOrderStateMachine
	m_pOrderProcessor->AddPortfolioOrderPlacer(this);

	// Direction
	SetDirection(posiDirection);

	// Limit price
	SetLimitPrice(pLmtPxArr, iPxSize);

	// Just do it;
	Do();
}

void CPortfolioOrderPlacer::Send()
{
	bool m_isSequential = m_pPortf->EnablePrefer();
	if(m_isSequential)
	{
		// Only start the first one
		m_sendingIdx = 0;
		assert(m_inputOrderPlacers.size() > 0);
		(m_inputOrderPlacers.at(0))->Do();
	}
	else
	{
		BOOST_FOREACH(const boost::shared_ptr<CInputOrderPlacer>& placer, m_inputOrderPlacers)
		{
			placer->Do();
		}
	}
}

bool CPortfolioOrderPlacer::SendNext( COrderEvent* transEvent )
{
	if(++m_sendingIdx < (int)m_inputOrderPlacers.size())
	{
		boost::shared_ptr<CInputOrderPlacer>& pSgPlacer = m_inputOrderPlacers.at(m_sendingIdx);

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

bool CPortfolioOrderPlacer::OnEnter( ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState )
{
	LOG_DEBUG(logger, boost::str(boost::format("MultiLeg Order(%s) enter %s") 
		% Id() % PrintState(state)));
	
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
			else if (m_inputOrderPlacers.size() > 1)
				break;	// if there is only 1 order in portfolio, recognized as partially filled
			// otherwise got to complete state
		}
		//break;
	case ORDER_STATE_COMPLETE:
		{
			trade::MlOrderOffset offset = m_multiLegOrderTemplate->offset();
			if(offset == trade::ML_OF_OPEN || offset == trade::ML_OF_OTHER)
				m_pPortf->AddPosition(*m_multiLegOrderTemplate);
			else if(offset == trade::ML_OF_CLOSE)
				m_pPortf->RemovePosition(*m_multiLegOrderTemplate);

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
			m_multiLegOrderTemplate->set_haswarn(true);

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
		m_isWorking = false;

	return isTerminal;
}

void CPortfolioOrderPlacer::UpdateMultiLegOrder()
{
	m_pOrderProcessor->PublishMultiLegOrderUpdate(m_multiLegOrderTemplate.get());
}

void CPortfolioOrderPlacer::OutputStatus( const string& statusMsg )
{
	m_multiLegOrderTemplate->set_statusmsg(statusMsg);
	UpdateMultiLegOrder();
}



