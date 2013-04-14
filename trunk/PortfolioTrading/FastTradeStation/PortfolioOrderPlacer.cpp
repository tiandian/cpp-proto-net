#include "PortfolioOrderPlacer.h"
#include "InputOrder.h"
#include "Portfolio.h"
#include "OrderProcessor2.h"
#include "globalmembers.h"
#include "orderhelper.h"
#include "OrderEvent.h"
#include "charsetconvert.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for func_state and func_state_machine
#include <boost/msm/front/euml/state_grammar.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

namespace // Concrete FSM implementation
{
	struct OrderPlacer_ : public msm::front::state_machine_def<OrderPlacer_>
	{
		CPortfolioOrderPlacer* m_pPlacer;

		// constructor with arguments
		OrderPlacer_(CPortfolioOrderPlacer* pPlacer):m_pPlacer(pPlacer)
		{}

		// state machines also have entry/exit actions 
		template <class Event, class Fsm> 
		void on_entry(Event const&, Fsm& fsm) 
		{
			// Set Order placer pointer to Submachine Sent
			OrderPlacer_::Sent& s = fsm.get_state<OrderPlacer_::Sent&>();
			s.m_pPlacer = fsm.m_pPlacer;
		} 

		// States definition
		struct Sending : public msm::front::state<> 
		{
			// every (optional) entry/exit methods get the event passed.
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& fsm) 
			{
				LOG_DEBUG(logger, "entering: Sending");
				fsm.m_pPlacer->OnSend();
			}

			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Sending"); }
		};

		struct LegOrderFilled : public msm::front::state<> 
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
				LOG_DEBUG(logger, "entering: LegOrderFilled");
				fsm.m_pPlacer->OnFilled(evt.m_pOrd);
			}

			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: LegOrderFilled"); }
		};

		struct LegOrderCanceled : public msm::front::state<> 
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
				fsm.m_pPlacer->OnLegCanceled(evt.m_pOrd);
			}

			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
		};

		struct LegOrderRejected : public msm::front::terminate_state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
				fsm.m_pPlacer->OnLegRejected(evt.m_pOrd);
			}

			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
		};

		struct Completed :  public msm::front::terminate_state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& fsm) 
			{
				fsm.m_pPlacer->OnCompleted();
			}
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
		};

		struct Canceled :  public msm::front::terminate_state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& fsm) 
			{
				fsm.m_pPlacer->OnPortfolioCanceled();
			}
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
		};

		struct AllOk : public msm::front::state<> {};

		struct Error :  public msm::front::terminate_state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
				fsm.m_pPlacer->OnError(evt.m_ErrorMsg);
			}
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
		};
		
		struct Sent_ : public msm::front::state_machine_def<Sent_>
		{
			CPortfolioOrderPlacer* m_pPlacer;

			Sent_():m_pPlacer(NULL){}

			struct Accepted : public msm::front::state<> 
			{
				// every (optional) entry/exit methods get the event passed.
				template <class Event,class FSM>
				void on_entry(Event const& evt,FSM& fsm) 
				{
					LOG_DEBUG(logger, "entering: Accepted");
					fsm.m_pPlacer->OnAccept(evt.m_pOrd);
				}

				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Accepted"); }
			};

			struct Pending : public msm::front::state<> 
			{
				template <class Event,class FSM>
				void on_entry(Event const& evt,FSM& fsm) 
				{ 
					LOG_DEBUG(logger, "entering: Pending");
					fsm.m_pPlacer->OnPending(evt.m_pOrd);
				}

				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Pending"); }
			};

			struct PartiallyFilled : public msm::front::state<> 
			{
				template <class Event,class FSM>
				void on_entry(Event const& evt, FSM& fsm) 
				{ 
					LOG_DEBUG(logger, "entering: PartiallyFilled");
					fsm.m_pPlacer->OnPartiallyFilled(evt.m_pOrd);
				}

				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: PartiallyFilled"); }
			};

			struct Canceling : public msm::front::state<> 
			{
				template <class Event,class FSM>
				void on_entry(Event const& evt, FSM& fsm) 
				{ 
					LOG_DEBUG(logger, "entering: Canceling");
					fsm.m_pPlacer->OnCanceling();
				}

				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Canceling"); }
			};

			struct Canceling2 : public msm::front::state<> {};

			// the initial state. Must be defined
			typedef Accepted initial_state;

			// Transition table for OrderPlacer
			struct transition_table : mpl::vector<
				//    Start				Event				Next			Action                     Guard
				//  +------------+--------------------+-----------------+---------------------------+----------------------+
				_row < Accepted  , evtPending	      , Pending		    >,
				_row < Accepted  , evtPartiallyFilled , PartiallyFilled >,
				_row < Accepted  , evtSubmit	      , Accepted		>,
				_row < Pending	 , evtPendingTimeUp   , Canceling		>,
				_row < Pending	 , evtNextQuoteIn	  , Canceling		>,
				_row < Pending   , evtPending	      , Pending			>,
				_row < Pending	 , evtPartiallyFilled , PartiallyFilled >,
				_row < Canceling , evtPending	      , Canceling2		>
			> {};

		};
		// back-end
		typedef msm::back::state_machine<Sent_> Sent;

		typedef mpl::vector<Sending, AllOk> initial_state;

		// transitions

		// Transition table for OrderPlacer
		struct transition_table : mpl::vector<
			//    Start					Event					Next			Action                     Guard
			//  +-------------------+-------------------+-------------------+---------------------------+----------------------+
			_row < Sending			, evtSubmit			, Sent				>,
			_row < Sending			, evtSubmitFailure	, Error			    >,
			_row < Sent				, evtFilled			, LegOrderFilled	>,
			_row < Sent				, evtCancelSuccess	, LegOrderCanceled	>,
			_row < Sent				, evtCancelFailure	, Error			    >,
			_row < Sent				, evtReject			, LegOrderRejected	>,
			_row < LegOrderFilled	, evtAllFilled		, Completed			>,
			_row < LegOrderFilled	, evtNextLeg		, Sending		    >,
			_row < LegOrderCanceled	, evtRetry			, Sending			>,
			_row < LegOrderCanceled	, evtFirstCanceled	, Canceled			>,
			_row < LegOrderCanceled	, evtFilledCanceled	, Error				>,
			_row < AllOk			, evtErrorFound		, Error				>
		> {};
		// Replaces the default no-transition response.
		template <class FSM,class Event>
		void no_transition(Event const& e, FSM& fsm, int state)
		{
			LOG_DEBUG(logger, boost::str(boost::format(
				 "no transition from state %d on event %s")
				 % state % typeid(e).name()));
			fsm.process_event(evtErrorFound("遇到无法处理的事件"));
		}
	};
	// Pick a back-end
	typedef msm::back::state_machine<OrderPlacer_> OrderPlacerFsm;
}

CPortfolioOrderPlacer::CPortfolioOrderPlacer(void)
	: m_pPortf(NULL)
	, m_pOrderProcessor(NULL)
	, m_isWorking(false)
	, m_isReady(false)
	, m_isSequential(false)
	, m_isClosingOrder(false)
	, m_sendingIdx(0)
	, m_submitTimes(0)
	, m_maxRetryTimes(0)
{
	m_fsm = boost::shared_ptr<void>(new OrderPlacerFsm(this));
}

CPortfolioOrderPlacer::~CPortfolioOrderPlacer(void)
{
}

void CPortfolioOrderPlacer::Initialize( const string& mlOrdId )
{
	m_multiLegOrderTemplate->set_orderid(mlOrdId);
	m_multiLegOrderTemplate->set_openorderid(mlOrdId);

	m_multiLegOrderTemplate->set_haswarn(false);
	m_multiLegOrderTemplate->set_statusmsg("");
}

void CPortfolioOrderPlacer::Prepare()
{
	BuildTemplateOrder();

	// Create input orders from template multi-leg order, then m_inputOrders will have items
	int inputOrdCount = GenInputOrders();

	bool autoTracking = m_pPortf->AutoTracking();
	m_maxRetryTimes = m_pPortf->RetryTimes();

	m_isReady = true;
}

int CPortfolioOrderPlacer::GenInputOrders()
{
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
			m_inputOrders.insert(m_inputOrders.begin(), inputOrder);
		else
			m_inputOrders.push_back(inputOrder);
	}

	return m_inputOrders.size();
}

void CPortfolioOrderPlacer::Run(trade::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize)
{
	if(!m_isReady)
		Prepare();

	m_isWorking = true;

	// Generate order Id
	string mlOrderId;
	m_pPortf->NewOrderId(mlOrderId);
	Initialize(mlOrderId);

	// Direction
	SetDirection(posiDirection);

	// Limit price
	SetLimitPrice(pLmtPxArr, iPxSize);

	// Sending the first leg
	m_sendingIdx = 0;

	// start fsm
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->start();
}

void CPortfolioOrderPlacer::OnSend()
{
	if(m_submitTimes > 0)
	{
		ModifyOrderPrice();
	}
		
	// lock and generate order ref
	int iOrdRef = m_pOrderProcessor->LockForSubmit(m_sendingOrderRef);
	assert(iOrdRef > 0);
		
	assert(m_sendingIdx >= 0);
	assert(m_sendingIdx < (int)m_inputOrders.size());

	CInputOrder* pInputOrder = m_inputOrders[m_sendingIdx].get();
	pInputOrder->set_orderref(m_sendingOrderRef);

	// Add this to order processor, so that return order can get back
	m_pOrderProcessor->AddOrderPlacer(this);

	LOG_INFO(logger, boost::str(boost::format("Submit Order(%s - %s) [No. %d time(s)]")
		% m_multiLegOrderTemplate->orderid() % pInputOrder->Symbol() % (m_submitTimes + 1)));

	// real submit order and unlock to allow next order ref generation
	bool succ = m_pOrderProcessor->SubmitAndUnlock(pInputOrder);

	++m_submitTimes;

	if(m_sendingIdx == 0 && m_submitTimes == 1)	// Only publish it for the first time
		UpdateMultiLegOrder();
}

void CPortfolioOrderPlacer::OnAccept(trade::Order* pRtnOrder)
{
	UpdateLegOrder(pRtnOrder);
}

void CPortfolioOrderPlacer::OnPending( trade::Order* pRtnOrder )
{
	if(SetPendingOrderInfo(pRtnOrder))
	{
		if(m_pendingOrderInfo.offsetFlag == trade::OF_OPEN)
		{
			int openTimeout = m_pPortf->OpenPendingTimeout();
			m_openOrderTimer = boost::shared_ptr<CAsyncOpenOrderTimer>(
				new CAsyncOpenOrderTimer(this, m_pendingOrderInfo.ordRef, openTimeout));
			m_openOrderTimer->Run();
		}
		else
		{
			m_isClosingOrder = true;
		}

		UpdateLegOrder(pRtnOrder);
	}
	else
		LOG_DEBUG(logger, boost::str(boost::format("Duplicate pending order event.(ordRef: %s, sysId: %s)")
					% pRtnOrder->orderref() % pRtnOrder->ordersysid()));
}

void CPortfolioOrderPlacer::OnFilled( trade::Order* pRtnOrder )
{
	++m_sendingIdx;
	if(m_sendingIdx < (int)m_inputOrders.size())
	{
		AfterLegDone();
		// Go to send next order
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtNextLeg());
	}
	else
	{
		// All leg order done
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtAllFilled());
	}

	UpdateLegOrder(pRtnOrder);
}

void CPortfolioOrderPlacer::OnPartiallyFilled( trade::Order* pRtnOrder )
{
	int remained = pRtnOrder->volumetotal();
	int finished = pRtnOrder->volumetraded();

	SetPendingOrderInfo(pRtnOrder);
	// Cancel remaining
	OnCanceling();
	UpdateLegOrder(pRtnOrder);
}

void CPortfolioOrderPlacer::OnCompleted()
{
	trade::MlOrderOffset offset = m_multiLegOrderTemplate->offset();
	if(offset == trade::ML_OF_OPEN || offset == trade::ML_OF_OTHER)
		m_pPortf->AddPosition(*m_multiLegOrderTemplate);
	else if(offset == trade::ML_OF_CLOSE)
		m_pPortf->RemovePosition(*m_multiLegOrderTemplate);

	AfterPortfolioDone();
}

void CPortfolioOrderPlacer::OnCanceling()
{
	LOG_DEBUG(logger, boost::str(boost::format("Canceling order (ref:%s, sysId:%s)")
		% m_pendingOrderInfo.ordRef % m_pendingOrderInfo.ordSysId));
	m_pOrderProcessor->CancelOrder(m_pendingOrderInfo.ordRef, 
		m_pendingOrderInfo.exchId, m_pendingOrderInfo.ordSysId, 
		m_pendingOrderInfo.userId, m_pendingOrderInfo.symbol);
}

void CPortfolioOrderPlacer::OnLegCanceled( trade::Order* pRtnOrder )
{
	AfterLegDone(true);

	if(m_sendingIdx == 0)
	{
		UpdateLegOrder(pRtnOrder);
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtFirstCanceled());
	}
	else if(m_submitTimes < m_maxRetryTimes)
	{
		// retry
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtRetry());
		UpdateLegOrder(pRtnOrder);
	}
	else
	{
		LOG_INFO(logger, boost::str(boost::format("Retry times is used up. Order(%s) has been retried %d times")
			% pRtnOrder->instrumentid() % m_submitTimes));
		
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(
			evtFilledCanceled("单腿:平仓失败"));
	}
}

void CPortfolioOrderPlacer::OnLegRejected( trade::Order* pRtnOrder )
{
	AfterLegDone();
	UpdateLegOrder(pRtnOrder);
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtErrorFound(pRtnOrder->statusmsg()));
}

void CPortfolioOrderPlacer::OnPortfolioCanceled()
{
	AfterPortfolioDone();
}

void CPortfolioOrderPlacer::OnError(const string& errMsg)
{
	AfterLegDone();
	AfterPortfolioDone();

	string ordStatusMsg;
	GB2312ToUTF_8(ordStatusMsg, errMsg.c_str());

	m_multiLegOrderTemplate->set_haswarn(true);
	m_multiLegOrderTemplate->set_statusmsg(ordStatusMsg);

	UpdateMultiLegOrder();
}

void CPortfolioOrderPlacer::OnPendingTimeUp()
{
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtPendingTimeUp());
}

void CPortfolioOrderPlacer::OnQuoteReceived( entity::Quote* pQuote )
{
	if(m_isClosingOrder)
	{
		m_nextQuote.last = pQuote->last();
		m_nextQuote.ask = pQuote->ask();
		m_nextQuote.bid = pQuote->bid();

		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtNextQuoteIn());
		m_isClosingOrder = false;
	}
}

void CPortfolioOrderPlacer::ModifyOrderPrice()
{
	CInputOrder* pInputOrder = m_inputOrders[m_sendingIdx].get();
	trade::TradeDirectionType direction = pInputOrder->Direction();
	double basePx = 0;
	if(direction == trade::BUY)
	{
		basePx = m_nextQuote.bid;
	}
	else
		basePx = m_nextQuote.ask;

	LOG_DEBUG(logger, boost::str(boost::format("In coming new quote's %s : %f") 
		% (direction == trade::BUY ? "Bid" : "Ask") % basePx));

	double priceTick = 0.2;

	if(direction == trade::BUY)
	{
		double buy = basePx + priceTick;
		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Buy @ %f")
			% pInputOrder->Symbol() % buy));
		pInputOrder->set_limitprice(buy);
	}
	else if(direction == trade::SELL)
	{
		double sell = basePx - priceTick;
		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Sell @ %f")
			% pInputOrder->Symbol() % sell));
		pInputOrder->set_limitprice(sell);
	}
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

void CPortfolioOrderPlacer::OnOrderReturned( trade::Order* pRtnOrder )
{
	trade::OrderSubmitStatusType submitStatus = pRtnOrder->ordersubmitstatus();
	trade::OrderStatusType status = pRtnOrder->orderstatus();

	LOG_DEBUG(logger, boost::str(boost::format("Order(%s, %s) - submit status(%s), order status(%s)")
		% pRtnOrder->orderref() %  pRtnOrder->ordersysid() % GetSumbitStatusText(submitStatus) % GetStatusText(status)));

	if(submitStatus > trade::NOT_SUBMITTED && 
		submitStatus <= trade::ACCEPTED && status >= trade::STATUS_UNKNOWN)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtSubmit(pRtnOrder));
	}
	else if(submitStatus > trade::ACCEPTED)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtReject(pRtnOrder));
	}
	else if(status == trade::ALL_TRADED)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtFilled(pRtnOrder));
	}
	else if(status == trade::ORDER_CANCELED)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtCancelSuccess(pRtnOrder));
	}
	else if(status == trade::NO_TRADE_QUEUEING ||
		status == trade::NO_TRADE_NOT_QUEUEING)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtPending(pRtnOrder));
	}
	else if(status == trade::PART_TRADED_QUEUEING ||
		status == trade::PART_TRADED_NOT_QUEUEING)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtPartiallyFilled(pRtnOrder));
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot identify status of order (ref: %s, sysId: %s)") 
			% pRtnOrder->orderref() % pRtnOrder->ordersysid()));
	}
}

void CPortfolioOrderPlacer::UpdateLegOrder( trade::Order* pRtnOrder )
{
	if(pRtnOrder != NULL)
	{
		trade::Order* pLegOrder = GetOrderBySymbol(pRtnOrder->instrumentid(), pRtnOrder->direction());

		pLegOrder->CopyFrom(*pRtnOrder);

		string ordStatusMsg;
		GB2312ToUTF_8(ordStatusMsg, pRtnOrder->statusmsg().c_str());
		pLegOrder->set_statusmsg(ordStatusMsg);

		m_pOrderProcessor->PublishOrderUpdate(m_multiLegOrderTemplate->portfolioid(), 
			m_multiLegOrderTemplate->orderid(), pLegOrder);
	}
}

trade::Order* CPortfolioOrderPlacer::GetOrderBySymbol(const string& symbol, trade::TradeDirectionType direction)
{
	trade::Order* pOrdFound = NULL;
	int count = m_multiLegOrderTemplate->legs_size();
	google::protobuf::RepeatedPtrField<trade::Order>* legs = m_multiLegOrderTemplate->mutable_legs();
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

bool CPortfolioOrderPlacer::SetPendingOrderInfo( trade::Order* pRtnOrder )
{
	if(pRtnOrder == NULL)
	{
		m_pendingOrderInfo.ordRef.clear();
		m_pendingOrderInfo.exchId.clear();
		m_pendingOrderInfo.ordSysId.clear(); 
		m_pendingOrderInfo.userId.clear();
		m_pendingOrderInfo.symbol.clear();
		m_pendingOrderInfo.offsetFlag = 0;
	}
	else
	{
		if(m_pendingOrderInfo.ordRef == pRtnOrder->orderref())
			return false;

		m_pendingOrderInfo.ordRef = pRtnOrder->orderref();
		m_pendingOrderInfo.exchId = pRtnOrder->exchangeid();
		m_pendingOrderInfo.ordSysId = pRtnOrder->ordersysid(); 
		m_pendingOrderInfo.userId = pRtnOrder->userid();
		m_pendingOrderInfo.symbol = pRtnOrder->instrumentid();
		m_pendingOrderInfo.offsetFlag = pRtnOrder->comboffsetflag()[0];
	}

	return true;
}

void CPortfolioOrderPlacer::Reset()
{
	m_maxRetryTimes = 0;
	m_isClosingOrder = false;
	m_sendingOrderRef.clear();
	m_openOrderTimer.reset();
	SetPendingOrderInfo(NULL);
}

void CPortfolioOrderPlacer::AfterLegDone( bool isCanceled /*= false*/ )
{
	if(!isCanceled)
		m_submitTimes = 0;

	if(m_openOrderTimer.get() != NULL)
		m_openOrderTimer->Cancel();
	
	if(!m_sendingOrderRef.empty())
		m_pOrderProcessor->RemoveOrderPlacer(Id());
	
	Reset();
}

void CPortfolioOrderPlacer::AfterPortfolioDone()
{
	m_isWorking = false;
	m_isClosingOrder = false;
	m_sendingIdx = 0;
}

void CPortfolioOrderPlacer::OnOrderPlaceFailed( const string& errMsg )
{
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtSubmitFailure(errMsg));
}

void CPortfolioOrderPlacer::OnOrderCancelFailed( const string& errMsg )
{
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtCancelFailure(errMsg));
}

void CPortfolioOrderPlacer::Cleanup()
{
	AfterLegDone();
	AfterPortfolioDone();

	m_multiLegOrderTemplate.reset();
	m_inputOrders.clear();
}








