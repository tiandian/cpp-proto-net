#include "SgOrderStateMachine.h"
#include "globalmembers.h"
#include "OrderProcessor2.h"
#include "charsetconvert.h"
#include "Portfolio.h"
#include "ScalperStrategy.h"
#include "ScalperOrderPlacer.h"

#include <boost/format.hpp>

using namespace op2;

string CSgOrderPlacer::EmptyParentOrderId;

trade::Order* GetOrderBySymbol(trade::MultiLegOrder* mlOrder, const string& symbol, trade::TradeDirectionType direction)
{
	trade::Order* pOrdFound = NULL;
	int count = mlOrder->legs_size();
	google::protobuf::RepeatedPtrField<trade::Order>* legs = mlOrder->mutable_legs();
	for(int i = 0; i < count; ++i)
	{
		trade::Order* pOrd = legs->Mutable(i);
		if(pOrd->instrumentid() == symbol && pOrd->direction() == direction)
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
	Initialize();
}


CSgOrderStateMachine::~CSgOrderStateMachine(void)
{
}

void CSgOrderStateMachine::Initialize()
{
	OrderStatePtr sending(new OrderSending);
	m_orderStates.insert(make_pair(sending->State(), sending));
	OrderStatePtr sent(new OrderSent);
	m_orderStates.insert(make_pair(sent->State(), sent));
	OrderStatePtr pending(new OrderPending);
	m_orderStates.insert(make_pair(pending->State(), pending));
	OrderStatePtr complete(new OrderComplete);
	m_orderStates.insert(make_pair(complete->State(), complete));
	OrderStatePtr failed(new OrderPlaceFailed);
	m_orderStates.insert(make_pair(failed->State(), failed));
	OrderStatePtr partiallyFilled(new OrderPartiallyFilled);
	m_orderStates.insert(make_pair(partiallyFilled->State(), partiallyFilled));
	OrderStatePtr canceling(new OrderCanceling);
	m_orderStates.insert(make_pair(canceling->State(), canceling));

	sending->AddEventState(ORDER_EVENT_SUBMIT_SUCCESS, sent.get());
	sending->AddEventState(ORDER_EVENT_REJECTED, failed.get());
	sending->AddEventState(ORDER_EVENT_SUBMIT_FAILED, failed.get());

	sent->AddEventState(ORDER_EVENT_SUBMIT_SUCCESS, sent.get());
	sent->AddEventState(ORDER_EVENT_COMPLETE, complete.get());
	sent->AddEventState(ORDER_EVENT_PENDING, pending.get());
	sent->AddEventState(ORDER_EVENT_REJECTED, failed.get());
	sent->AddEventState(ORDER_EVENT_PARTIALLY_FILLED, partiallyFilled.get());

	pending->AddEventState(ORDER_EVENT_CANCEL_FAILED, failed.get());
	pending->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, sending.get());
	pending->AddEventState(ORDER_EVENT_PENDING, pending.get());
	pending->AddEventState(ORDER_EVENT_COMPLETE, complete.get());
	pending->AddEventState(ORDER_EVENT_PARTIALLY_FILLED, partiallyFilled.get());

	pending->AddEventState(ORDER_EVENT_PENDING_TIMEUP, canceling.get());
	pending->AddEventState(ORDER_EVENT_NEXT_QUOTE_IN, canceling.get());

	canceling->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, sending.get());
	canceling->AddEventState(ORDER_EVENT_CANCEL_FAILED, failed.get());

	partiallyFilled->AddEventState(ORDER_EVENT_CANCEL_SUCCESS, complete.get());
	partiallyFilled->AddEventState(ORDER_EVENT_COMPLETE, complete.get());
}

void CSgOrderStateMachine::Transition( const string& orderId/*orderRef*/, COrderEvent* event )
{
	COrderStateMachine::Transition(orderId, event);
}

CSgOrderPlacer* CSgOrderStateMachine::CreateScalperPlacer( CPortfolio* pPortfolio, trade::MultiLegOrder* pMultiLegOrder, const InputOrderPtr& pInputOrder, int retryTimes, COrderProcessor2* pOrderProc )
{
	return new CScalperOrderPlacer(this, pPortfolio, pMultiLegOrder, pInputOrder, retryTimes, pOrderProc);
}

void CSgOrderPlacer::Do()
{
	COrderState* sentState = m_pStateMachine->GetState(ORDER_STATE_SENDING);
	sentState->Run(this, NULL);
}

bool CSgOrderPlacer::OnEnter( ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState )
{
	string dbText = boost::str(boost::format("Order(%s - %s) enter %s") 
		% ParentOrderId() % Symbol() % PrintState(state));
	logger.Debug(dbText);
	bool isTerminal = false;

	CSgOrderEvent* pSgOrderEvent = NULL;
	if(ORDER_STATE_SENDING != state)
	{
		pSgOrderEvent = dynamic_cast<CSgOrderEvent*>(transEvent);
		assert(pSgOrderEvent != NULL);
		if(pSgOrderEvent == NULL)
			return isTerminal;
	}

	switch(state)
	{
	case ORDER_STATE_SENDING:
		{
			if(m_submitTimes <= m_maxRetryTimes)
			{
				OrderPlacerPtr tmp_self;

				if(m_submitTimes > 0)
				{
					bool removeSucc = m_pStateMachine->RemovePlacer(Id(), &tmp_self);
					ModifyOrderPrice();
				}
				else
				{
					tmp_self = OrderPlacerPtr(this);
				}

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
					m_pStateMachine->AddPlacer(tmp_self);

					string submitInfo = boost::str(boost::format("Submit Order(%s - %s) [No. %d time(s)]")
						% ParentOrderId() % Symbol() % (m_submitTimes + 1));
					logger.Info(submitInfo);

					OnSubmittingOrder();

					// real submit order and unlock to allow next order ref generation
					bool succ = m_pOrderProcessor->SubmitAndUnlock(m_pInputOrder.get());

					if(m_pPortf != NULL)
					{
						CLeg* pLeg = m_pPortf->GetLeg(Symbol());
						assert(pLeg != NULL);
						if(pLeg != NULL)
							m_quoteTimestamp = pLeg->GetTimestamp();
					}

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

				
				CSgOrderEvent* pSgOrderEvent = dynamic_cast<CSgOrderEvent*>(transEvent);
				trade::Order* pOrd = pSgOrderEvent->RtnOrder();
				if(pOrd != NULL)
				{
					OnOrderUpdate(pOrd);
				}
				LegCanceledEvent legCxlEvt(Symbol());
				RaiseMultiLegOrderEvent(&legCxlEvt);
			}
		}
		break;
	case ORDER_STATE_SENT:
		{
			trade::Order* pOrd = pSgOrderEvent->RtnOrder();
			if(pOrd != NULL)
			{
				OnOrderUpdate(pOrd);
			}
		}
		break;
	case ORDER_STATE_PENDING:
		{
			trade::Order* pOrd = pSgOrderEvent->RtnOrder();
			if(pOrd != NULL)
			{
				OnPending(pOrd);
				
				OnOrderUpdate(pOrd);
			}
		}
		break;
	case ORDER_STATE_CANCELING:
		{
			OnCanceling();
		}
		break;
	case ORDER_STATE_COMPLETE:
		{
			trade::Order* pOrd = pSgOrderEvent->RtnOrder();
			int remained = pOrd->volumetotal();
			int finished = pOrd->volumetraded();
			OnOrderUpdate(pOrd);
			m_succ = true;
			isTerminal = true;

			LegCompletedEvent legCompletedEvt(Symbol(), remained, finished);
			RaiseMultiLegOrderEvent(&legCompletedEvt);
		}
		break;
	case ORDER_STATE_PLACE_FAILED:
		{
			m_succ = false;

			if(pSgOrderEvent != NULL)
			{
				OnOrderPlaceFailed(pSgOrderEvent);
			}
			else
			{
				m_errorMsg = "Order not completed due to unexpected event";
			}
			isTerminal = true;

			LegRejectedEvent legRejEvt(Symbol());
			RaiseMultiLegOrderEvent(&legRejEvt);
		}
		break;
	case ORDER_STATE_PARTIALLY_FILLED:
		{
			// Need to do same thing as order pending
			trade::Order* pOrd = pSgOrderEvent->RtnOrder();
			if(pOrd != NULL)
			{
				if(!m_allowPending)
				{
					const std::string& ordRef = pOrd->orderref();
					const std::string& exchId = pOrd->exchangeid();
					const std::string& ordSysId = pOrd->ordersysid(); 
					const std::string& userId = pOrd->userid();
					const std::string& symbol = pOrd->instrumentid();
					m_pOrderProcessor->CancelOrder(ordRef, exchId, ordSysId, userId, symbol);
				}
				OnOrderUpdate(pOrd);
			}
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
		trade::Order* pLegOrder = GetOrderBySymbol(m_pMultiLegOrder, pOrd->instrumentid(), pOrd->direction());

		pLegOrder->CopyFrom(*pOrd);

		string ordStatusMsg;
		GB2312ToUTF_8(ordStatusMsg, pOrd->statusmsg().c_str());
		pLegOrder->set_statusmsg(ordStatusMsg);

		m_pOrderProcessor->PublishOrderUpdate(m_pMultiLegOrder->portfolioid(), m_pMultiLegOrder->orderid(), pLegOrder);
	}
}

void CSgOrderPlacer::ModifyOrderPrice()
{
	CLeg* pLeg = m_pPortf->GetLeg(Symbol());
	assert(pLeg != NULL);
	if(pLeg != NULL)
	{
		bool quoteUpdated = pLeg->IsQuoteUpdated(m_quoteTimestamp);
		if(!quoteUpdated)
		{
			logger.Warning(boost::str(boost::format("Order(%s)'s quote didn't get updated after cancelled")
				% Symbol()));
		}

		trade::TradeDirectionType direction = m_pInputOrder->direction();
		double origLmtPx = m_pInputOrder->limitprice();
		if(direction == trade::BUY)
		{
			double ask = pLeg->Ask();
			logger.Trace(boost::str(boost::format("Buy: Ask(%f) ?> Lmt Px(%f)")
				% ask % origLmtPx));
			logger.Trace(boost::str(boost::format("Modify order(%s): Buy @ %f")
				% Symbol() % ask));
			m_pInputOrder->set_limitprice(ask);
		}
		else if(direction == trade::SELL)
		{
			double bid = pLeg->Bid();
			logger.Trace(boost::str(boost::format("Sell: Bid(%f) ?< Lmt Px(%f)")
				% bid % origLmtPx));
			logger.Trace(boost::str(boost::format("Modify order(%s): Sell @ %f")
				% Symbol() % bid));
			m_pInputOrder->set_limitprice(bid);
		}
	}
	
}

void CSgOrderPlacer::RaiseMultiLegOrderEvent( COrderEvent* orderEvent )
{
	m_pOrderProcessor->RaiseMLOrderPlacerEvent(ParentOrderId(), orderEvent);
}

void CSgOrderPlacer::OnOrderPlaceFailed( COrderEvent* pOrdEvent )
{
	CSgOrderEvent* pSgOrderEvent = dynamic_cast<CSgOrderEvent*>(pOrdEvent);
	trade::Order* pOrd = pSgOrderEvent->RtnOrder();
	if(pOrd != NULL)
	{
		OnOrderUpdate(pOrd);
	}
	else
	{
		trade::Order* pLegOrder = GetOrderBySymbol(m_pMultiLegOrder, 
			m_pInputOrder->instrumentid(), m_pInputOrder->direction());

		pLegOrder->set_orderref(m_currentOrdRef);
		pLegOrder->set_ordersubmitstatus(trade::INSERT_REJECTED);
		const string& errorMsg = pSgOrderEvent->StatusMsg();
		if(!errorMsg.empty())
		{
			GB2312ToUTF_8(m_errorMsg, errorMsg.c_str());
		}
		else
		{
			m_errorMsg = "Order not completed";
		}
		pLegOrder->set_statusmsg(m_errorMsg);

		m_pOrderProcessor->PublishOrderUpdate(m_pMultiLegOrder->portfolioid(), 
			m_pMultiLegOrder->orderid(), pLegOrder);
	}
}

void CSgOrderPlacer::AdjustQuantity( int qty )
{
	m_pInputOrder->set_volumetotaloriginal(qty);
}

void CSgOrderPlacer::OnPending( trade::Order* pOrd )
{
	if(!m_allowPending)
	{
		CancelOrder(pOrd);
	}
}

void CSgOrderPlacer::CancelOrder( trade::Order* pOrd )
{
	const std::string& ordRef = pOrd->orderref();
	const std::string& exchId = pOrd->exchangeid();
	const std::string& ordSysId = pOrd->ordersysid(); 
	const std::string& userId = pOrd->userid();
	const std::string& symbol = pOrd->instrumentid();
	m_pOrderProcessor->CancelOrder(ordRef, exchId, ordSysId, userId, symbol);
}

void CManualSgOrderPlacer::ModifyOrderPrice()
{
	entity::Quote* pQuote = NULL;
	bool succ = m_pOrderProcessor->QuerySymbol(Symbol(), &pQuote);
	if(succ)
	{
		trade::TradeDirectionType direction = m_pInputOrder->direction();
		double origLmtPx = m_pInputOrder->limitprice();
		if(direction == trade::BUY)
		{
			double ask = pQuote->ask();
			logger.Trace(boost::str(boost::format("Buy: Ask(%f) ?> Lmt Px(%f)")
				% ask % origLmtPx));
			logger.Trace(boost::str(boost::format("Modify order(%s): Buy @ %f")
				% Symbol() % ask));
			m_pInputOrder->set_limitprice(ask);
		}
		else if(direction == trade::SELL)
		{
			double bid = pQuote->bid();
			logger.Trace(boost::str(boost::format("Sell: Bid(%f) ?< Lmt Px(%f)")
				% bid % origLmtPx));
			logger.Trace(boost::str(boost::format("Modify order(%s): Sell @ %f")
				% Symbol() % bid));
			m_pInputOrder->set_limitprice(bid);
		}
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot query quote %s") % Symbol()));
	}
}

bool CManualSgOrderPlacer::DoAndWait()
{
	boost::unique_lock<boost::mutex> l(m_mut);

	Do();

	if(!m_cond.timed_wait(l, boost::posix_time::seconds(30)))
	{
		m_errorMsg = "Manual close order timeout";
		return false;
	}

	return m_succ;
}

void CManualSgOrderPlacer::RaiseMultiLegOrderEvent( COrderEvent* orderEvent )
{
	boost::mutex::scoped_lock l(m_mut);
	m_cond.notify_one();
}

void CManualSgOrderPlacer::OnOrderPlaceFailed( COrderEvent* pOrdEvent )
{
	const string& errorMsg = pOrdEvent->StatusMsg();
	if(!errorMsg.empty())
	{
		GB2312ToUTF_8(m_errorMsg, errorMsg.c_str());
	}
	else
	{
		m_errorMsg = "Order not completed";
	}
}
