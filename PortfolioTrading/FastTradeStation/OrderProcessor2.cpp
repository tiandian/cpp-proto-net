#include "OrderProcessor2.h"
#include "globalmembers.h"
#include "orderhelper.h"
#include "TradeAgent.h"
#include "ClientAgent.h"
#include "charsetconvert.h"
#include "SymbolInfoRepositry.h"

#include <boost/format.hpp>
#include <boost/date_time.hpp>

extern CSymbolInfoRepositry symbolInfoRepo;

const char EX_SHFE[] = "SHFE";
const char EX_DCE[] = "DCE";
const char EX_CZCE[] = "CZCE";
const char EX_FFEX[] = "FFEX";

const char* TRADE_DIRECTION[] = {"Buy", "Sell"};

void PrintInputOrder(trade::InputOrder* order)
{
	if(order != NULL)
	{
		string orderInfo = boost::str(boost::format("%s %s @ %.2f")
			% TRADE_DIRECTION[order->direction() - trade::BUY]
			% order->instrumentid() % order->limitprice());
		logger.Info(orderInfo);
	}
}

COrderProcessor2::COrderProcessor2(void):
m_pTradeAgent(NULL),
m_pClientAgent(NULL),
m_maxOrderRef(0),
m_bIsSubmitting(false),
m_totalOpenTimes(0),
m_totalCancelTimes(0),
m_maxTotalOpenTimes(450),
m_maxTotalCancelTimes(900)
{
}


COrderProcessor2::~COrderProcessor2(void)
{
}

void COrderProcessor2::SubmitPortfOrder( CPortfolio* pPortf, const MultiLegOrderPtr& multilegOrder )
{
	COrderPlacer* pOrdPlacer = m_mlOrderStateMachine.CreatePlacer(pPortf, multilegOrder, this);
	pOrdPlacer->Do();
}

void COrderProcessor2::OnRspUserLogin( bool succ, std::string& msg, int initOrderRefID )
{
	if(succ)
		m_maxOrderRef = initOrderRefID;
	else
	{
		string warn = "Trade login failed due to " + msg;
		logger.Warning(warn);
	}
}

void COrderProcessor2::OnRspOrderInsert( bool succ, const std::string& orderRef, const std::string& msg )
{
	m_sgOrderStateMachine.Transition(orderRef, SubmitFailedEvent(msg));
}

void COrderProcessor2::OnRspOrderAction( bool succ, const std::string& orderRef, const std::string& msg )
{
	if(succ)
		logger.Info(boost::str(boost::format("Cancel order(%s) succeeded.") % orderRef));
	else
	{
		logger.Info(boost::str(boost::format("Cancel order(%s) failed. message: %s") 
			% orderRef.c_str() % msg.c_str()));

		m_sgOrderStateMachine.Transition(orderRef, CancelFailedEvent(NULL));
	}
}

void COrderProcessor2::OnRtnOrder( trade::Order* order )
{
	COrderEvent* pEvent = NULL;
	bool succ = GetOrderEvent(order, &pEvent);
	if(succ)
	{
		boost::shared_ptr<COrderEvent> orderEvtPtr(pEvent);
		string ordRef = order->orderref();
		m_sgOrderStateMachine.Transition(ordRef, *pEvent);
	}
}

void COrderProcessor2::OnRtnTrade( trade::Trade* pTrade )
{
	PublishTradeUpdate(pTrade);
}

void COrderProcessor2::OnRspQryInvestorPositionDetail( trade::PositionDetailInfo* pPositionDetail )
{
	PublishPositionDetail(pPositionDetail);
}

void COrderProcessor2::PrintOrderStatus( trade::Order* order )
{
	trade::OrderSubmitStatusType submitStatus = order->ordersubmitstatus();
	trade::OrderStatusType status = order->orderstatus();
	logger.Debug(boost::str(boost::format("Order(%s) - submit status(%s), order status(%s)")
		% order->orderref().c_str() % GetSumbitStatusText(submitStatus) % GetStatusText(status)));
}

bool COrderProcessor2::GetOrderEvent( trade::Order* order, COrderEvent** ppOrderEvt )
{
	*ppOrderEvt = NULL;

	trade::OrderSubmitStatusType submitStatus = order->ordersubmitstatus();
	trade::OrderStatusType status = order->orderstatus();
	logger.Debug(boost::str(boost::format("Order(%s) - submit status(%s), order status(%s)")
		% order->orderref().c_str() % GetSumbitStatusText(submitStatus) % GetStatusText(status)));

	if(submitStatus > trade::NOT_SUBMITTED && 
		submitStatus <= trade::ACCEPTED && status >= trade::STATUS_UNKNOWN)
	{
		*ppOrderEvt = new SubmitSuccessEvent(order);
	}
	if(submitStatus > trade::ACCEPTED)
	{
		*ppOrderEvt = new RejectEvent(order);
	}
	else if(status == trade::ALL_TRADED)
	{
		*ppOrderEvt = new CompleteEvent(order);
	}
	else if(status == trade::ORDER_CANCELED)
	{
		*ppOrderEvt = new CancelSuccessEvent(order);
	}
	else if(status == trade::NO_TRADE_QUEUEING ||
		status == trade::NO_TRADE_NOT_QUEUEING)
	{
		*ppOrderEvt = new PendingEvent(order);
	}
	else if(status == trade::PART_TRADED_QUEUEING ||
		status == trade::PART_TRADED_NOT_QUEUEING)
	{
		*ppOrderEvt = new PartiallyFilledEvent(order);
	}

	return *ppOrderEvt != NULL;
}

void COrderProcessor2::CancelOrder( const std::string& ordRef, const std::string& exchId, const std::string& ordSysId, const std::string& userId, const std::string& symbol )
{
	boost::shared_ptr<trade::InputOrderAction> orderAction(new trade::InputOrderAction);

	orderAction->set_orderref(ordRef);

	///操作标志
	orderAction->set_actionflag(trade::AF_Delete);	// Cancel order

	///交易所代码
	orderAction->set_exchangeid(exchId);
	///报单编号
	orderAction->set_ordersysid(ordSysId);
	///用户代码
	orderAction->set_userid(userId);

	orderAction->set_instrumentid(symbol);

	if(m_pTradeAgent != NULL)
	{
		if(!ReachCancelTimesLimit())
		{
			m_pTradeAgent->SubmitOrderAction(orderAction.get());
			AddCancelTimes();
		}
	}
}

CSgOrderPlacer* COrderProcessor2::CreateSingleOrderPlacer(CPortfolio* pPortf, trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes)
{
	if(pMlOrder->reason() == trade::SR_Scalpe)
	{
		// if scalpe open order, don't need to retry
		int actRetryTimes = pInputOrder->comboffsetflag() == "0" ? 0 : retryTimes;
		return m_sgOrderStateMachine.CreateScalperPlacer(pPortf, pMlOrder, pInputOrder, actRetryTimes, this);
	}
	else
		return m_sgOrderStateMachine.CreatePlacer(pPortf, pMlOrder, pInputOrder, retryTimes, this);
}

void COrderProcessor2::RaiseMLOrderPlacerEvent( const string& mlOrdPlacerId, COrderEvent& orderEvent )
{
	m_mlOrderStateMachine.Transition(mlOrdPlacerId, orderEvent);
}

int COrderProcessor2::LockForSubmit( string& outOrdRef )
{
	int retOrderRef = -1;
	boost::unique_lock<boost::mutex> lock(m_mutOrdRefIncr);
	if(m_condSubmit.timed_wait(lock, boost::posix_time::seconds(3), 
		boost::bind(&COrderProcessor2::IsReadyForSubmit, this)))
	{
		retOrderRef = GenerateOrderRef(outOrdRef);
		m_bIsSubmitting = true;
	}
	else
		logger.Warning("LOCK for submit order TIME OUT!");

	return retOrderRef;
}

bool COrderProcessor2::SubmitAndUnlock( trade::InputOrder* pOrder )
{
	boost::mutex::scoped_lock lock(m_mutOrdRefIncr);

	PrintInputOrder(pOrder);

	bool succ = SubmitOrderToTradeAgent(pOrder);
	m_bIsSubmitting = false;
	m_condSubmit.notify_one();
	return succ;
}

int COrderProcessor2::GenerateOrderRef( string& outOrdRef )
{
	static char orderRef[10];
	sprintf_s(orderRef, "%d", m_maxOrderRef);
	outOrdRef = orderRef;
	int currOrdRef = m_maxOrderRef++;
	return currOrdRef;
}

bool COrderProcessor2::SubmitOrderToTradeAgent( trade::InputOrder* pOrder )
{
	logger.Trace(boost::str(boost::format("Truly sumbit order(%s) to trade agent") 
		% pOrder->orderref()));
	bool succ = m_pTradeAgent->SubmitOrder(pOrder);
	if(succ)
		AddOpenTimes();
	return succ;
}

void COrderProcessor2::PublishMultiLegOrderUpdate( trade::MultiLegOrder* pOrder )
{
	m_pClientAgent->OnMultiLegOrderUpdated(pOrder);
}

void COrderProcessor2::PublishOrderUpdate( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	m_pClientAgent->OnLegOrderUpdated(portfId, mlOrderId, legOrd);
}

void COrderProcessor2::PublishTradeUpdate( trade::Trade* pTrade )
{
	m_pClientAgent->OnTradeUpdated(pTrade);
}

void COrderProcessor2::PublishPositionDetail( trade::PositionDetailInfo* pPosiDetailInfo )
{
	m_pClientAgent->OnPostionDetailReturned(pPosiDetailInfo);
}

void COrderProcessor2::Initialize( CClientAgent* pClientAgent, CTradeAgent* pTradeAgent )
{
	m_pClientAgent = pClientAgent;
	m_pTradeAgent = pTradeAgent;
	if(m_pTradeAgent != NULL)
		m_pTradeAgent->SetCallbackHanlder(this);
}

void COrderProcessor2::OnRspQryTradingAccount( const trade::AccountInfo& accountInfo )
{
	boost::mutex::scoped_lock lock(m_mutQryAcct);
	accountInfo.SerializeToString(&m_serializedQryAcctInfo);
	m_condQryAcct.notify_one();
}

bool COrderProcessor2::QueryAccountInfo( string* outSerializedAcctInfo )
{
	bool ret = false;
	boost::unique_lock<boost::mutex> lock(m_mutQryAcct);
	m_serializedQryAcctInfo.clear();
	m_pTradeAgent->QueryAccount();

	if(m_condQryAcct.timed_wait(lock, boost::posix_time::seconds(15)))
	{
		*outSerializedAcctInfo = m_serializedQryAcctInfo;
		ret = true;
	}

	return ret;
}

void COrderProcessor2::QueryPositionDetails( const string& symbol )
{
	m_pTradeAgent->QueryPositionDetails(symbol);
}

trade::OffsetFlagType GetTradeOffset(const string& exchId, boost::gregorian::date openDate, boost::gregorian::date tradingDay)
{
	if(exchId == EX_SHFE)
	{
		if(openDate < tradingDay)
			return trade::OF_CLOSE;
		else
			return trade::OF_CLOSE_TODAY;
	}
	else
		return trade::OF_CLOSE;
}

trade::InputOrder* COrderProcessor2::BuildCloseOrder( const string& symbol, trade::TradeDirectionType direction, const string& openDate, PlaceOrderContext* placeOrderCtx )
{
	entity::Quote* pQuote = NULL;
	bool succ = m_pTradeAgent->QuerySymbol(symbol, &pQuote);
	if(succ)
	{
		logger.Info(boost::str(boost::format("Query Quote %s: %d") 
			% symbol.c_str() % pQuote->last()));

		double limitPrice = direction == trade::SELL ? pQuote->bid(): pQuote->ask();

		boost::gregorian::date openDt = boost::gregorian::from_undelimited_string(openDate);
		string exchId = symbolInfoRepo.GetExchangeId(symbol);
		trade::OffsetFlagType offsetFlag = GetTradeOffset(exchId, openDt, m_pTradeAgent->TradingDay());

		trade::InputOrder* closeOrder(
			BuildSingleOrder(symbol, limitPrice, direction, offsetFlag, placeOrderCtx));
		
		delete pQuote;

		return closeOrder;
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot query quote %s") % symbol));
	}
	return NULL;
}

boost::tuple<bool, string> COrderProcessor2::PlaceOrder( const string& symbol, trade::TradeDirectionType direction, const string& openDate, PlaceOrderContext* placeOrderCtx )
{
	boost::shared_ptr<trade::InputOrder> pInputOrder(BuildCloseOrder(symbol, direction, openDate, placeOrderCtx));
	if(pInputOrder.get() == NULL)
		return boost::make_tuple(false, string("Failed to build close order"));

	CManualSgOrderPlacer* placer = m_sgOrderStateMachine.CreateManualPlacer(pInputOrder, 2, this);
	
	bool succ = placer->DoAndWait();

	string errMsg = placer->GetError();
	
	string pId = placer->Id();
	m_sgOrderStateMachine.RemovePlacer(pId);

	return boost::make_tuple(succ, errMsg);
}

bool COrderProcessor2::QuerySymbol( const std::string& symbol, entity::Quote** ppQuote )
{
	return m_pTradeAgent->QuerySymbol(symbol, ppQuote);
}
