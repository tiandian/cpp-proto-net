#include "StdAfx.h"
#include "PortfolioOrderPlacer.h"
#include "InputOrder.h"
#include "Portfolio.h"
#include "OrderProcessor.h"
#include "globalmembers.h"
#include "orderhelper.h"
#include "OrderEvent.h"
#include "charsetconvert.h"
#include "BuildOrderException.h"

#include <boost/date_time.hpp>

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

string PendingTimeUpEventName("struct evtPendingTimeUp");
string NextQuoteInEventName("struct evtNextQuoteIn");

namespace // Concrete FSM implementation
{
	struct OrderPlacer_ : public msm::front::state_machine_def<OrderPlacer_>
	{
		typedef int activate_deferred_events;

		CPortfolioOrderPlacer* m_pPlacer;

		// constructor with arguments
		OrderPlacer_(CPortfolioOrderPlacer* pPlacer):m_pPlacer(pPlacer)
		{}

		// state machines also have entry/exit actions 
		template <class Event, class Fsm> 
		void on_entry(Event const&, Fsm& fsm) 
		{
			// Set Order placer pointer to Submachine Sent
			OrderPlacer_::Sent& s = fsm.template get_state<OrderPlacer_::Sent&>();
			s.m_pPlacer = fsm.m_pPlacer;
		} 

		// States definition
		struct Sending : public msm::front::state<> 
		{
			typedef mpl::vector<evtPending> deferred_events;

			// every (optional) entry/exit methods get the event passed.
#ifdef LOG_FOR_TRADE			
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& fsm) 
			{
				LOG_DEBUG(logger, "entering: Sending");
			}
#endif

#ifdef LOG_FOR_TRADE
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Sending"); }
#endif
		};

		struct LegOrderFilled : public msm::front::state<> 
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, "entering: LegOrderFilled");
#endif
				fsm.m_pPlacer->OnFilled(evt.m_pOrd);
			}
#ifdef LOG_FOR_TRADE
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: LegOrderFilled"); }
#endif
		};

		struct LegOrderCanceled : public msm::front::state<> 
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
				fsm.m_pPlacer->OnLegCanceled(evt.m_pOrd);
			}
#ifdef LOG_FOR_TRADE
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
#endif
		};

		struct LegOrderRejected : public msm::front::state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
				fsm.m_pPlacer->OnLegRejected(evt.m_pOrd);
			}
#ifdef LOG_FOR_TRADE
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
#endif
		};

		struct Completed :  public msm::front::terminate_state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& fsm) 
			{
				fsm.m_pPlacer->OnCompleted();
			}
#ifdef LOG_FOR_TRADE
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
#endif
		};

		struct Canceled :  public msm::front::terminate_state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& fsm) 
			{
				fsm.m_pPlacer->OnPortfolioCanceled();
			}
#ifdef LOG_FOR_TRADE
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
#endif
		};

		struct AllOk : public msm::front::state<> {};

		struct Error :  public msm::front::terminate_state<>
		{
			template <class Event,class FSM>
			void on_entry(Event const& evt,FSM& fsm) 
			{
				fsm.m_pPlacer->OnError(evt.m_ErrorMsg);
			}
#ifdef LOG_FOR_TRADE
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) {}
#endif
		};
		
		struct Sent_ : public msm::front::state_machine_def<Sent_>
		{
			// if the evtNextQuoteIn event is fired while in this state, defer it until a state
			// handles or rejects it
			typedef mpl::vector<evtRetry> deferred_events;

			CPortfolioOrderPlacer* m_pPlacer;

			Sent_():m_pPlacer(NULL){}

			struct Accepted : public msm::front::state<> 
			{
				// every (optional) entry/exit methods get the event passed.
				template <class Event,class FSM>
				void on_entry(Event const& evt,FSM& fsm) 
				{
#ifdef LOG_FOR_TRADE
					LOG_DEBUG(logger, "entering: Accepted");
#endif
					fsm.m_pPlacer->OnAccept(evt.m_pOrd);
				}
#ifdef LOG_FOR_TRADE
				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Accepted"); }
#endif
			};

			struct Pending : public msm::front::state<> 
			{
				template <class Event,class FSM>
				void on_entry(Event const& evt,FSM& fsm) 
				{ 
#ifdef LOG_FOR_TRADE
					LOG_DEBUG(logger, "entering: Pending");
#endif
					fsm.m_pPlacer->OnPending(evt.m_pOrd);
				}
#ifdef LOG_FOR_TRADE
				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Pending"); }
#endif
			};

			struct PartiallyFilled : public msm::front::state<> 
			{
				template <class Event,class FSM>
				void on_entry(Event const& evt, FSM& fsm) 
				{ 
#ifdef LOG_FOR_TRADE
					LOG_DEBUG(logger, "entering: PartiallyFilled");
#endif
					fsm.m_pPlacer->OnPartiallyFilled(evt.m_pOrd);
				}
#ifdef LOG_FOR_TRADE
				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: PartiallyFilled"); }
#endif
			};

			struct Canceling : public msm::front::state<> 
			{
				template <class Event,class FSM>
				void on_entry(Event const& evt, FSM& fsm) 
				{ 
#ifdef LOG_FOR_TRADE
					LOG_DEBUG(logger, "entering: Canceling");
#endif
					fsm.m_pPlacer->OnCanceling();
				}
#ifdef LOG_FOR_TRADE
				template <class Event,class FSM>
				void on_exit(Event const&,FSM& ) { LOG_DEBUG(logger, "leaving: Canceling"); }
#endif
			};

			// transitions internal to Empty
			void duplicate_pending(evtPending const&)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, "Ignore duplicate pending"); 
#endif
			}

			void partiallyfilled_to_canceling(evtPartiallyFilled const& evt)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, "partially filled event on canceling"); 
#endif
				m_pPlacer->OnPartiallyFilledToCanceling(evt.m_pOrd);
			}

			// the initial state. Must be defined
			typedef Accepted initial_state;

			typedef Sent_ s;

			// Transition table for OrderPlacer
			struct transition_table : mpl::vector<
				//    Start				Event				Next			Action                     Guard
				//  +------------+--------------------+-----------------+---------------------------+----------------------+
				_row < Accepted  , evtPending	      , Pending		    >,
				_row < Accepted  , evtPartiallyFilled , PartiallyFilled >,
				_row < Accepted  , evtSubmit	      , Accepted		>,
			   _irow < Accepted  , evtNextQuoteIn						>,
				_row < Pending	 , evtPendingTimeUp   , Canceling		>,
				_row < Pending	 , evtNextQuoteIn	  , Canceling		>,
			  a_irow < Pending   , evtPending		  ,					   &s::duplicate_pending	>,
				_row < Pending	 , evtPartiallyFilled , PartiallyFilled >,
			  a_irow < Canceling , evtPending	      ,					   &s::duplicate_pending	>,
			  a_irow < Canceling , evtPartiallyFilled ,					   &s::partiallyfilled_to_canceling  >,
			   _irow < Canceling , evtNextQuoteIn						>,
			    _row < PartiallyFilled , evtPendingTimeUp   , Canceling	>,
				_row < PartiallyFilled , evtNextQuoteIn	    , Canceling	>,
			   _irow < PartiallyFilled , evtPartiallyFilled				>
			> {};

		};
		// back-end
		typedef msm::back::state_machine<Sent_> Sent;

		typedef mpl::vector<Sending, AllOk> initial_state;

		// transitions
		template <class Event>
		void on_send(Event const&)
		{
			m_pPlacer->Send();
		}
		
		void on_cancel_success(evtCancelSuccess const& evt)       
		{
			m_pPlacer->OnOrderCanceled(evt.m_pOrd);
		}

		// guards
		bool if_portfolio_canceled(evtCancelSuccess const&)
		{
			return m_pPlacer->IfPortfolioCanceled();
		}
		bool if_leg_canceled(evtCancelSuccess const&)
		{
			return !(m_pPlacer->IfPortfolioCanceled());
		}
		bool if_cancel_failed(evtCancelFailure const& evt)
		{
			// If Cancel failed due to order finished, DON'T go to Error
			return evt.m_ErrorId != 26;
		}
		typedef OrderPlacer_ p;

		// Transition table for OrderPlacer
		struct transition_table : mpl::vector<
			//    Start					Event					Next			Action                     Guard
			//  +-------------------+-------------------+-------------------+---------------------------+--------------------------+
			_row < Sending			, evtSubmit			, Sent				>,
			_row < Sending			, evtSubmitFailure	, Error			    >,
			 Row < Sending			, evtPending		, none				 , Defer					, none						>,
			_row < Sent				, evtFilled			, LegOrderFilled	>,
			 row < Sent				, evtCancelSuccess	, LegOrderCanceled	 , &p::on_cancel_success	, &p::if_leg_canceled		>,
			 row < Sent				, evtCancelSuccess	, Canceled			 , &p::on_cancel_success	, &p::if_portfolio_canceled >,
		   g_row < Sent				, evtCancelFailure	, Error											, &p::if_cancel_failed		>,
			_row < Sent				, evtReject			, LegOrderRejected	>,
			 Row < Sent				, evtRetry		    , none				 , Defer					, none						>,
			_row < LegOrderFilled	, evtAllFilled		, Completed			>,
		   a_row < LegOrderFilled	, evtNextLeg		, Sending		     , &p::on_send		       >,
		   a_row < LegOrderCanceled	, evtNextLeg		, Sending		     , &p::on_send		       >,
		   a_row < LegOrderCanceled	, evtRetry			, Sending			 , &p::on_send		       >,
			_row < LegOrderCanceled	, evtFilledCanceled	, Error				>,
			_row < AllOk			, evtErrorFound		, Error				>
		> {};
		// Replaces the default no-transition response.
		template <class FSM,class Event>
		void no_transition(Event const& e, FSM& fsm, int state)
		{
			string unExpectedEvtName = typeid(e).name();
			LOG_DEBUG(logger, boost::str(boost::format(
				"no transition from state %d on event %s")
				% state % unExpectedEvtName));

			if(PendingTimeUpEventName == unExpectedEvtName ||
				NextQuoteInEventName == unExpectedEvtName)
			{
				logger.Warning(boost::str(boost::format("Event %s is ignored") % unExpectedEvtName));
			}
			else
			{
				//fsm.process_event(evtErrorFound("遇到无法处理的事件"));
				logger.Error("Encounter unexpected event");
			}
		}
	};
	// Pick a back-end
	typedef msm::back::state_machine<OrderPlacer_> OrderPlacerFsm;
}

CPortfolioOrderPlacer::CPortfolioOrderPlacer(void)
	: m_pPortf(NULL)
	, m_pOrderProcessor(NULL)
	, m_activeOrdPlacer(NULL)
	, m_lastDoneOrdPlacer(NULL)
	, m_isReady(false)
	, m_isWorking(false)
	, m_isFirstLeg(false)
	, m_sendNextOnFilled(true)
	, m_posiDirection(entity::NET)
{
	m_fsm = boost::shared_ptr<void>(new OrderPlacerFsm(this));
}

CPortfolioOrderPlacer::~CPortfolioOrderPlacer(void)
{
	m_thCleanup.join();
}

void CPortfolioOrderPlacer::SetNewOrderId(const string& mlOrdId, const char* openOrdId)
{
	m_multiLegOrderTemplate->set_orderid(mlOrdId);
	if(openOrdId == NULL)
		m_multiLegOrderTemplate->set_openorderid(mlOrdId);
	else
		m_multiLegOrderTemplate->set_openorderid(openOrdId);
}

void CPortfolioOrderPlacer::ResetTemplate()
{
	m_multiLegOrderTemplate->set_haswarn(false);
	m_multiLegOrderTemplate->set_statusmsg("");

	// Clear leg order status
	int count = m_multiLegOrderTemplate->legs_size();
	google::protobuf::RepeatedPtrField<trade::Order>* legs = m_multiLegOrderTemplate->mutable_legs();
	for(int i = 0; i < count; ++i)
	{
		trade::Order* pOrd = legs->Mutable(i);
		pOrd->set_volumetotaloriginal(0);
		pOrd->set_volumetraded(0);
		pOrd->set_ordersubmitstatus(trade::NOT_SUBMITTED);
		pOrd->set_orderstatus(trade::STATUS_UNKNOWN);
		pOrd->set_statusmsg("");
		pOrd->set_inserttime("");
	}
}

void CPortfolioOrderPlacer::Prepare()
{
	boost::unique_lock<boost::mutex> l(m_mutCleaning);
	m_condCleanDone.wait(l, boost::bind(&CPortfolioOrderPlacer::IsReadyForPrepare, this));

	BuildTemplateOrder();

	// Create input orders from template multi-leg order, then m_inputOrders will have items
	GenLegOrderPlacers();
	
	SetFirstLeg();

	m_isReady = true;
}

CLegOrderPlacer* CPortfolioOrderPlacer::CreateLegOrderPlacer( int openTimeout, int maxRetryTimes )
{
	return new CLegOrderPlacer(this, openTimeout, maxRetryTimes);
}

void CPortfolioOrderPlacer::GenLegOrderPlacers()
{
	int openTimeout = m_pPortf->Strategy()->OpenTimeout();
	int maxRetryTimes = m_pPortf->Strategy()->RetryTimes();

	int legIdx = 0;
	BOOST_FOREACH(const trade::Order& o, m_multiLegOrderTemplate->legs())
	{
		boost::shared_ptr<CLegOrderPlacer> legOrderPlacer(CreateLegOrderPlacer(openTimeout, maxRetryTimes));
		
		legOrderPlacer->InputOrder().set_brokerid(o.brokerid());
		legOrderPlacer->InputOrder().set_investorid(o.investorid());
		legOrderPlacer->InputOrder().set_symbol(o.instrumentid());
		legOrderPlacer->InputOrder().set_orderref(o.orderref());
		legOrderPlacer->InputOrder().set_userid(o.userid());
		legOrderPlacer->InputOrder().set_orderpricetype(o.orderpricetype());
		legOrderPlacer->InputOrder().set_direction(o.direction());
		legOrderPlacer->InputOrder().set_combhedgeflag(o.combhedgeflag());
		legOrderPlacer->InputOrder().set_comboffsetflag(o.comboffsetflag());
		legOrderPlacer->InputOrder().set_limitprice(o.limitprice());
		legOrderPlacer->InputOrder().set_volumetotaloriginal(o.volumetotaloriginal());
		legOrderPlacer->SetVolumeOriginal(o.volumetotaloriginal());
		legOrderPlacer->InputOrder().set_timecondition(o.timecondition());
		legOrderPlacer->InputOrder().set_gtddate(o.gtddate());
		legOrderPlacer->InputOrder().set_volumecondition(	o.volumecondition());
		legOrderPlacer->InputOrder().set_minvolume(o.minvolume());
		legOrderPlacer->InputOrder().set_contingentcondition(	o.contingentcondition());
		legOrderPlacer->InputOrder().set_stopprice(o.stopprice());
		legOrderPlacer->InputOrder().set_forceclosereason(o.forceclosereason());
		legOrderPlacer->InputOrder().set_isautosuspend(o.isautosuspend());
		legOrderPlacer->InputOrder().set_businessunit(o.businessunit());
		legOrderPlacer->InputOrder().set_requestid(o.requestid());
		legOrderPlacer->InputOrder().set_userforceclose(o.userforceclose());

		legOrderPlacer->LegIndex(legIdx++);
		
		// Give derived a chance to set custom properties of leg order placer
		OnAddingLegOrderPlacer(legOrderPlacer.get());

		if(o.preferred())	// always put preferred order at front
		{
			m_legPlacers.insert(m_legPlacers.begin(), legOrderPlacer);
		}
		else
		{
			m_legPlacers.push_back(legOrderPlacer);
		}
	}
	
	unsigned int seq = 0;
	for(vector<LegOrderPlacerPtr>::iterator iter = m_legPlacers.begin();
		iter != m_legPlacers.end(); ++iter, ++seq)
	{
		(*iter)->SequenceNo(seq);
	}
}

void CPortfolioOrderPlacer::Run(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize,
		const boost::chrono::steady_clock::time_point& trigQuoteTimestamp)
{
	if(!m_isReady)
		Prepare();

	m_isWorking.store(true, boost::memory_order_release);

	// Direction
	SetDirection(posiDirection);

	// Limit price
	SetLimitPrice(pLmtPxArr, iPxSize);

	m_triggingTimestamp = trigQuoteTimestamp;

	// Start fsm, fsm goes into Sending status
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->start();

	// And Sending the first leg
	Send();
}

void CPortfolioOrderPlacer::GoStart(const char* openOrderId)
{
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->start();
	Send(openOrderId);
}

void CPortfolioOrderPlacer::Run( entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize )
{
	Run(posiDirection, pLmtPxArr, iPxSize, boost::chrono::steady_clock::now());
}

void CPortfolioOrderPlacer::Send(const char* openOrderId)
{
	assert(m_activeOrdPlacer != NULL);
#ifdef LOG_FOR_TRADE
	LOG_DEBUG(logger, boost::str(boost::format("Sending Active Order placer's Id:%d ") 
		% m_activeOrdPlacer->SequenceNo()));
#endif
	
	// lock and generate order ref
	string sendingOrderRef;
	int iOrdRef = m_pOrderProcessor->LockForSubmit(sendingOrderRef);
	assert(iOrdRef > 0);

	// set order ref to input order to be sent
	m_activeOrdPlacer->OrderRef(sendingOrderRef);

	// Add this to order processor, so that return order can get back
	m_pOrderProcessor->AddOrderPlacer(this);

	// real submit order and unlock to allow next order ref generation
	bool succ = m_pOrderProcessor->SubmitAndUnlock(&(m_activeOrdPlacer->InputOrder()));

	m_activeOrdPlacer->AddSubmitTimes();

	boost::chrono::steady_clock::duration elapsed = 
		boost::chrono::steady_clock::now() - m_triggingTimestamp;
	long usElapse = boost::chrono::duration_cast<boost::chrono::microseconds>(elapsed).count();

	if(m_isFirstLeg && m_activeOrdPlacer->SubmitTimes() == 1)	// Only publish it for the first time
	{
		// Generate order Id
		string mlOrderId;
		m_pPortf->NewOrderId(mlOrderId);
		SetNewOrderId(mlOrderId, openOrderId);

		LOG_INFO(logger, boost::str(boost::format("%s [%s] Submit Order(%s - %s, OrderRef: %d, vol: %d) [No. %d time(s)] in %d us after the last QUOTE")
			% m_pOrderProcessor->InvestorId()
			% ((m_activeOrdPlacer->InputOrder().OffsetFlag()[0] == trade::OF_OPEN) ? "OPEN" : "CLOSE")
			% m_multiLegOrderTemplate->orderid() % m_activeOrdPlacer->Symbol() 
			% iOrdRef % (m_activeOrdPlacer->InputOrder()).VolumeTotalOriginal()
			% m_activeOrdPlacer->SubmitTimes() % usElapse));

		ResetTemplate();
		m_isFirstLeg = false;
		FillSendingOrderNote();
		PushWholeMultiLegOrder(m_multiLegOrderTemplate.get());
		ResetSendingOrderNote();
	}
	else
	{
		LOG_INFO(logger, boost::str(boost::format("%s [%s] Submit Order(%s - %s, OrderRef: %d, vol: %d) [No. %d time(s)] in %d us after the last QUOTE")
			% m_pOrderProcessor->InvestorId()
			% ((m_activeOrdPlacer->InputOrder().OffsetFlag()[0] == trade::OF_OPEN) ? "OPEN" : "CLOSE")
			% m_multiLegOrderTemplate->orderid() % m_activeOrdPlacer->Symbol() 
			% iOrdRef % (m_activeOrdPlacer->InputOrder()).VolumeTotalOriginal()
			% m_activeOrdPlacer->SubmitTimes() % usElapse));

		// if second leg
		if(m_sendNextOnFilled)
			UpdateLastDoneOrder();
	}

	OnLegOrderSent( m_activeOrdPlacer->SequenceNo());
}

void CPortfolioOrderPlacer::OnAccept(const RtnOrderWrapperPtr& pRtnOrder)
{
	UpdateLegOrder(pRtnOrder);
}

void CPortfolioOrderPlacer::OnPending(const RtnOrderWrapperPtr& pRtnOrder )
{
	assert(m_activeOrdPlacer != NULL);
	if(!m_activeOrdPlacer->IsPending())
	{
		m_activeOrdPlacer->StartPending(pRtnOrder);

		UpdateLegOrder(pRtnOrder);
	}
	else
		LOG_DEBUG(logger, boost::str(boost::format("Duplicate pending order event.(ordRef: %s, sysId: %s)")
					% pRtnOrder->OrderRef() % pRtnOrder->OrderSysId()));
}

void CPortfolioOrderPlacer::OnFilled(const RtnOrderWrapperPtr& pRtnOrder )
{
	// The first thing is to cancel pending timer
	m_activeOrdPlacer->CancelPending();
	AfterLegDone();
	
	unsigned int sendingIdx = m_activeOrdPlacer->SequenceNo();
#ifdef LOG_FOR_TRADE
	LOG_DEBUG(logger, boost::str(boost::format("No.%d OrderPlacer gets filled") % sendingIdx));
#endif
	++sendingIdx;
	if(sendingIdx < m_legPlacers.size())
	{
		m_activeOrdPlacer->UpdateOrder(pRtnOrder);
		m_lastDoneOrdPlacer = m_activeOrdPlacer;
		// Go to send next order
		m_activeOrdPlacer = m_legPlacers[sendingIdx].get();
		m_triggingTimestamp = pRtnOrder->Timestamp();
		if(m_sendNextOnFilled)
			GotoNext();
		else
			UpdateLastDoneOrder();
	}
	else
	{
		UpdateLegOrder(pRtnOrder);
		// All leg order done
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtAllFilled());
	}
}

void CPortfolioOrderPlacer::OnPartiallyFilled(const RtnOrderWrapperPtr& pRtnOrder )
{
	if(!m_activeOrdPlacer->IsPending())
	{
		m_activeOrdPlacer->StartPending(pRtnOrder);
	}

	int remained = pRtnOrder->VolumeTotal();
	int finished = pRtnOrder->VolumeTraded();
	
	m_activeOrdPlacer->PartiallyFill(finished);
	UpdateLegOrder(pRtnOrder);
}

void CPortfolioOrderPlacer::OnPartiallyFilledToCanceling(const RtnOrderWrapperPtr& pRtnOrder)
{
	int remained = pRtnOrder->VolumeTotal();
	int finished = pRtnOrder->VolumeTraded();

	m_activeOrdPlacer->PartiallyFill(finished);
}

void CPortfolioOrderPlacer::OnOrderCanceled(const RtnOrderWrapperPtr& pRtnOrder )
{
	UpdateLegOrder(pRtnOrder);
	m_activeOrdPlacer->Reset(true);
	m_pOrderProcessor->RemoveOrderPlacer(Id());
	// Count cancel volume
	int remained = pRtnOrder->VolumeTotal();
	// only count cancel TIMES instead of volumes
	m_pPortf->IncrementalCancelTimes(1);
}

void CPortfolioOrderPlacer::OnCompleted()
{
	trade::MlOrderOffset offset = m_multiLegOrderTemplate->offset();
	if(offset == trade::ML_OF_OPEN || offset == trade::ML_OF_OTHER)
		m_pPortf->AddPosition(*m_multiLegOrderTemplate);
	else if(offset == trade::ML_OF_CLOSE)
		m_pPortf->RemovePosition(*m_multiLegOrderTemplate);
	
	m_pPortf->UpdatePosition();

	AfterPortfolioDone(PortfolioFilled);
}

void CPortfolioOrderPlacer::OnCanceling()
{
	assert(m_activeOrdPlacer != NULL);
	assert(m_activeOrdPlacer->IsPending());
#ifdef LOG_FOR_TRADE
	LOG_DEBUG(logger, boost::str(boost::format("Canceling order (ref:%s, sysId:%s)")
		% m_activeOrdPlacer->OrderRef() % m_activeOrdPlacer->OrderSysId()));
#endif
	
	m_pOrderProcessor->CancelOrder(m_activeOrdPlacer->OrderRef(), 
		m_activeOrdPlacer->ExchId(), m_activeOrdPlacer->OrderSysId(), 
		m_activeOrdPlacer->UserId(), m_activeOrdPlacer->Symbol());
}

void CPortfolioOrderPlacer::OnLegCanceled(const RtnOrderWrapperPtr& pRtnOrder )
{
	assert(m_activeOrdPlacer != NULL);

	int remained = pRtnOrder->VolumeTotal();
	int finished = pRtnOrder->VolumeTraded();
	if(finished > 0)	// partially fill order has been canceled
	{
		LOG_INFO(logger, boost::str(boost::format("OrderRef(%s) canceled as %d/%d filled")
			% m_activeOrdPlacer->OrderRef() % finished % pRtnOrder->VolumeTotalOriginal()));

		if(m_activeOrdPlacer->IsOpen())
		{
			// adjust next order placer volume
			m_activeOrdPlacer->Reset();
			
			unsigned int sendingIdx = m_activeOrdPlacer->SequenceNo();
			LOG_DEBUG(logger, boost::str(boost::format("No.%d OrderPlacer(OPEN) gets partially filled") % sendingIdx));
			++sendingIdx;
			if(sendingIdx < m_legPlacers.size())
			{
				// Go to send next order
				m_activeOrdPlacer = m_legPlacers[sendingIdx].get();
				m_activeOrdPlacer->AdjustVolume(finished);
				GotoNext();
			}
			else
			{
				logger.Error("Partially filled order is not OPEN order ???");
			}
		}
		else // if close
		{
			m_activeOrdPlacer->AdjustVolume(remained);
			LOG_DEBUG(logger, boost::str(boost::format("Partially filled close Order adjust volume from %d to %d, And Goto Retry...")
				% pRtnOrder->VolumeTotalOriginal() % pRtnOrder->VolumeTotal()));
			GotoRetry(pRtnOrder);
		}
	}
	else
	{
		GotoRetry(pRtnOrder);
	}
}

void CPortfolioOrderPlacer::OnQuoteReceived( boost::chrono::steady_clock::time_point& quoteTimestamp, entity::Quote* pQuote )
{
	boost::lock_guard<boost::mutex> l(m_mutOuterAccessFsm);
	
	if(m_activeOrdPlacer == NULL)
		return;	// in case already cleaned up

	if(m_activeOrdPlacer->Symbol() != pQuote->symbol())
		return; // in case other symbol's quote coming

	// if order placer is closing order and retry times available
	if(!(m_activeOrdPlacer->IsOpen()) && m_activeOrdPlacer->CanRetry())
	{
		bool needCancel = m_activeOrdPlacer->ModifyPrice(pQuote);
		if(needCancel)
		{
#ifdef LOG_FOR_TRADE
			LOG_DEBUG(logger, boost::str(boost::format("Notify Pending Order of Placer(No.%d) to Cancel") % m_activeOrdPlacer->SequenceNo()));
#endif
			boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtNextQuoteIn());

		}
		m_triggingTimestamp = quoteTimestamp;
	}
}

void CPortfolioOrderPlacer::OnLegRejected(const RtnOrderWrapperPtr& pRtnOrder )
{
	AfterLegDone();
	UpdateLegOrder(pRtnOrder);
	
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtErrorFound(pRtnOrder->StatusMsg()));
}

void CPortfolioOrderPlacer::OnPortfolioCanceled()
{
	AfterLegDone();
	AfterPortfolioDone(PortfolioCanceled);
}

void CPortfolioOrderPlacer::OnError(const string& errMsg)
{
	AfterLegDone();
	AfterPortfolioDone(PortfolioError);

	string ordStatusMsg;
	GB2312ToUTF_8(ordStatusMsg, errMsg.c_str());

	m_multiLegOrderTemplate->set_haswarn(true);
	m_multiLegOrderTemplate->set_statusmsg(ordStatusMsg);

	UpdateMultiLegOrder();
}

void CPortfolioOrderPlacer::OnPendingTimeUp()
{
	boost::lock_guard<boost::mutex> l(m_mutOuterAccessFsm);
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtPendingTimeUp());
}

void CPortfolioOrderPlacer::UpdateMultiLegOrder()
{
	// Copy order info from legOrder Placer
	BOOST_FOREACH(const LegOrderPlacerPtr& legPlacer, m_legPlacers)
	{
		int legIdx = legPlacer->LegIndex();
		assert(legIdx < m_multiLegOrderTemplate->legs_size());

		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(legIdx);
		if(legPlacer->IsOrderReady())
			pOrd->CopyFrom(legPlacer->Order());
	}
	
	PushWholeMultiLegOrder(m_multiLegOrderTemplate.get());
}

void CPortfolioOrderPlacer::OutputStatus( const string& statusMsg )
{
	string ordStatusMsg;
	GB2312ToUTF_8(ordStatusMsg, statusMsg.c_str());
	LOG_INFO(logger, boost::str(boost::format("OutputStatus %s -> %s") % statusMsg % ordStatusMsg));
	m_multiLegOrderTemplate->set_statusmsg(ordStatusMsg);
	UpdateMultiLegOrder();
	// reset status msg after sending
	m_multiLegOrderTemplate->set_statusmsg("");
}

void CPortfolioOrderPlacer::OnOrderReturned( RtnOrderWrapperPtr& rtnOrder )
{
	boost::lock_guard<boost::mutex> l(m_mutOuterAccessFsm);

	trade::OrderSubmitStatusType submitStatus = rtnOrder->OrderSubmitStatus();
	trade::OrderStatusType status = rtnOrder->OrderStatus();
#ifdef LOG_FOR_TRADE
	LOG_DEBUG(logger, boost::str(boost::format("Order(%s, %s) - submit status(%s), order status(%s)")
		% rtnOrder->OrderRef() %  rtnOrder->OrderSysId() % GetSumbitStatusText(submitStatus) % GetStatusText(status)));
#endif
	if(submitStatus > trade::NOT_SUBMITTED && 
		submitStatus <= trade::ACCEPTED && status >= trade::STATUS_UNKNOWN)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtSubmit(rtnOrder));
	}
	else if(submitStatus > trade::ACCEPTED)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtReject(rtnOrder));
	}
	else if(status == trade::ALL_TRADED)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtFilled(rtnOrder));
	}
	else if(status == trade::ORDER_CANCELED)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtCancelSuccess(rtnOrder));
	}
	else if(status == trade::NO_TRADE_QUEUEING ||
		status == trade::NO_TRADE_NOT_QUEUEING)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtPending(rtnOrder));
	}
	else if(status == trade::PART_TRADED_QUEUEING ||
		status == trade::PART_TRADED_NOT_QUEUEING)
	{
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtPartiallyFilled(rtnOrder));
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot identify status of order (ref: %s, sysId: %s)") 
			% rtnOrder->OrderRef() % rtnOrder->OrderSysId()));
	}
}

void CPortfolioOrderPlacer::UpdateLegOrder(const RtnOrderWrapperPtr& pRtnOrder )
{
	if(pRtnOrder.get() != NULL)
	{
		m_activeOrdPlacer->UpdateOrder(pRtnOrder);
		trade::Order& legOrder = m_activeOrdPlacer->OrderEntity();

		PushIndividualLegOrder(m_multiLegOrderTemplate->portfolioid(), 
			m_multiLegOrderTemplate->orderid(), &legOrder);
	}
}

void CPortfolioOrderPlacer::UpdateLastDoneOrder()
{
	if(m_lastDoneOrdPlacer != NULL)
	{
		trade::Order& legOrder = m_lastDoneOrdPlacer->OrderEntity();

		PushIndividualLegOrder(m_multiLegOrderTemplate->portfolioid(), 
			m_multiLegOrderTemplate->orderid(), &legOrder);

		m_lastDoneOrdPlacer = NULL;
	}
}

void CPortfolioOrderPlacer::AfterLegDone()
{
	m_activeOrdPlacer->Reset();
	m_pOrderProcessor->RemoveOrderPlacer(Id());
}

void CPortfolioOrderPlacer::AfterPortfolioDone(PortfolioFinishState portfState)
{
	m_isWorking.store(false, boost::memory_order_release);
	// Give portfolio a chance to check whether open times, position or cancel times reach limit
	m_pPortf->CheckOpenCancelLimit();

	OnPortfolioDone(portfState);
	// set first leg for next start
	SetFirstLeg();
}

void CPortfolioOrderPlacer::OnOrderPlaceFailed( const string& errMsg )
{
	boost::lock_guard<boost::mutex> l(m_mutOuterAccessFsm);
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtSubmitFailure(errMsg));
}

void CPortfolioOrderPlacer::OnOrderCancelFailed( int errorId, const string& errMsg )
{
	boost::lock_guard<boost::mutex> l(m_mutOuterAccessFsm);
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtCancelFailure(errorId, errMsg));
}

void CPortfolioOrderPlacer::Cleanup()
{
	m_thCleanup = boost::thread(boost::bind(&CPortfolioOrderPlacer::CleanupProc, this));
}

void CPortfolioOrderPlacer::CleanupProc()
{
	int retryTimes = 0;
	while(m_isWorking.load(boost::memory_order_acquire))
	{
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
		++retryTimes;
		if(retryTimes > 3)
			boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtErrorFound("Force terminate due to already wait 3 seconds"));
	}

	m_activeOrdPlacer = NULL;
	m_multiLegOrderTemplate.reset();
	m_legPlacers.clear();
	{
		boost::unique_lock<boost::mutex> l(m_mutCleaning);
		m_isReady = false;
		m_condCleanDone.notify_one();
	}
}

bool CPortfolioOrderPlacer::IfPortfolioCanceled()
{
	assert(m_activeOrdPlacer != NULL);
	return m_activeOrdPlacer->SequenceNo() == 0 && !m_activeOrdPlacer->IsPartiallyFilled();
}

void CPortfolioOrderPlacer::SetFirstLeg()
{
	ResetOrderPlacerStatus();
	m_activeOrdPlacer = m_legPlacers[0].get();
	m_isFirstLeg = true;
}

void CPortfolioOrderPlacer::GotoRetry(const RtnOrderWrapperPtr& pRtnOrder)
{
	if(m_activeOrdPlacer->CanRetry())
	{
#ifdef LOG_FOR_TRADE
		LOG_DEBUG(logger, boost::str(boost::format("Active Order Placer(No.%d) Go to retry ") % m_activeOrdPlacer->SequenceNo()));
#endif
		// retry
		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtRetry());
	}
	else
	{
		AfterLegDone();
		LOG_INFO(logger, boost::str(boost::format("Retry times is used up. Order(%s) has been retried %d times")
			% pRtnOrder->Symbol() % m_activeOrdPlacer->SubmitTimes()));

		boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(
			evtFilledCanceled("单腿:平仓失败"));
	}
}

void CPortfolioOrderPlacer::SetDirection( entity::PosiDirectionType posiDirection )
{
	m_posiDirection = posiDirection;

	static trade::TradeDirectionType LONG_TRADE_SEQ[2] = {trade::BUY, trade::SELL};
	static trade::TradeDirectionType SHORT_TRADE_SEQ[2] = {trade::SELL, trade::BUY};

	for(int i = 0; i < m_multiLegOrderTemplate->legs_size(); ++i)
	{
		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(i);

		// in case wanna open position
		if(posiDirection == entity::LONG)
		{
			// open long position
			pOrd->set_direction(LONG_TRADE_SEQ[i]);
			m_legPlacers[i]->InputOrder().set_direction(LONG_TRADE_SEQ[i]);
		}
		else if(posiDirection == entity::SHORT)
		{
			pOrd->set_direction(SHORT_TRADE_SEQ[i]);
			m_legPlacers[i]->InputOrder().set_direction(SHORT_TRADE_SEQ[i]);
		}
		else
		{
			throw CUnexpectedPositionDirectionException();
		}
	}
}

void CPortfolioOrderPlacer::SetLimitPrice( double* pLmtPxArr, int iPxSize )
{
	for(int i = 0; i < iPxSize; ++i)
	{
		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(i);
		double lmtPx = pLmtPxArr[i];
		pOrd->set_limitprice(lmtPx);
		m_legPlacers[i]->InputOrder().set_limitprice(lmtPx);
	}
}

void CPortfolioOrderPlacer::GotoNext()
{
	boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->process_event(evtNextLeg());
}

void CPortfolioOrderPlacer::ResetSendingOrderNote()
{
	m_sendingOrderNote.clear();
	m_multiLegOrderTemplate->set_statusmsg("");
}

void CPortfolioOrderPlacer::SetMlOrderStatus( const string& statusMsg )
{
	m_sendingOrderNote = statusMsg;
}

void CPortfolioOrderPlacer::FillSendingOrderNote()
{
	if(!m_sendingOrderNote.empty())
	{
		string ordStatusMsg;
		GB2312ToUTF_8(ordStatusMsg, m_sendingOrderNote.c_str());
		LOG_INFO(logger, boost::str(boost::format("FillSendingOrderNote %s -> %s") % m_sendingOrderNote % ordStatusMsg));
		m_multiLegOrderTemplate->set_statusmsg(ordStatusMsg);
	}
}

void CPortfolioOrderPlacer::ResetOrderPlacerStatus()
{
	// Copy order info from legOrder Placer
	BOOST_FOREACH(const LegOrderPlacerPtr& legPlacer, m_legPlacers)
	{
		legPlacer->ResetOrderEntityStatus();
	}
}

void CPortfolioOrderPlacer::PushWholeMultiLegOrder( trade::MultiLegOrder* pOrder )
{
	boost::unique_lock<boost::mutex> l(m_mutPushMlOrd);
	m_pOrderProcessor->PublishMultiLegOrderUpdate(pOrder);
	m_pushingMlOrdId = pOrder->orderid();
	m_condMlOrdPushed.notify_one();
}

void CPortfolioOrderPlacer::PushIndividualLegOrder( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	boost::unique_lock<boost::mutex> l(m_mutPushMlOrd);
	while(m_pushingMlOrdId.compare(mlOrderId) != 0)
	{
		bool got = m_condMlOrdPushed.timed_wait(l, boost::posix_time::seconds(1));
		if(!got)
		{
			logger.Error(boost::str(boost::format("Portfolio(%s) MLOrder(%s) Leg Order(%s) didn't get pushed due to Parent MLOrder not sent??")
				% portfId % mlOrderId % legOrd->orderref()));
			return;
		}
	}

	m_pOrderProcessor->PublishOrderUpdate(portfId, mlOrderId, legOrd);
}










