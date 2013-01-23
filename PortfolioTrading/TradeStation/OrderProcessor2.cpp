#include "StdAfx.h"
#include "OrderProcessor2.h"
#include "globalmembers.h"
#include "orderhelper.h"
#include "TradeAgent.h"
#include "charsetconvert.h"

#include <boost/format.hpp>


COrderProcessor2::COrderProcessor2(void):
m_pTradeAgent(NULL),
m_maxOrderRef(0),
m_bIsSubmitting(false)
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
		*ppOrderEvt == new PendingEvent(order);
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
		m_pTradeAgent->SubmitOrderAction(orderAction.get());
	}
}

COrderPlacer* COrderProcessor2::CreateSingleOrderPlacer(CPortfolio* pPortf, trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes)
{
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
	return m_pTradeAgent->SubmitOrder(pOrder);
}

void COrderProcessor2::SetPushPortfolioFunc( PushMultiLegOrderFunc funcPushMLOrder )
{
	m_pushMultiOrdFunc = funcPushMLOrder;
}

void COrderProcessor2::PublishMultiLegOrderUpdate( trade::MultiLegOrder* pOrder )
{
	if(!m_pushMultiOrdFunc.empty())
	{
		m_pushMultiOrdFunc(pOrder);
	}
}

void COrderProcessor2::SetPushOrderFunc( PushLegOrderFunc funcPushOrder )
{
	m_pushLegOrderFunc = funcPushOrder;
}

void COrderProcessor2::PublishOrderUpdate( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	if(!m_pushLegOrderFunc.empty())
	{
		m_pushLegOrderFunc(portfId, mlOrderId, legOrd);
	}
}

void COrderProcessor2::SetPushTradeFunc( PushTradeFunc funcPushTrade )
{
	m_pushTradeFunc = funcPushTrade;
}

void COrderProcessor2::PublishTradeUpdate( trade::Trade* pTrade )
{
	if(!m_pushTradeFunc.empty())
	{
		m_pushTradeFunc(pTrade);
	}
}

void COrderProcessor2::PublishPositionDetail( trade::PositionDetailInfo* pPosiDetailInfo )
{
	if(!m_pushPosiDetailFunc.empty())
	{
		m_pushPosiDetailFunc(pPosiDetailInfo);
	}
}

void COrderProcessor2::SetPushPositionDetailFunc( PushPositionDetailFunc funcPushPosiDetail )
{
	m_pushPosiDetailFunc = funcPushPosiDetail;
}

void COrderProcessor2::Initialize( CTradeAgent* pTradeAgent )
{
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

trade::InputOrder* COrderProcessor2::BuildCloseOrder( const string& symbol, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, PlaceOrderContext* placeOrderCtx )
{
	entity::Quote* pQuote = NULL;
	bool succ = m_pTradeAgent->QuerySymbol(symbol, &pQuote);
	if(succ)
	{
		logger.Info(boost::str(boost::format("Query Quote %s: %d") 
			% symbol.c_str() % pQuote->last()));

		double limitPrice = direction == trade::SELL ? pQuote->bid(): pQuote->ask();

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

boost::tuple<bool, string> COrderProcessor2::PlaceOrder( const string& symbol, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, PlaceOrderContext* placeOrderCtx )
{
	boost::shared_ptr<trade::InputOrder> pInputOrder(BuildCloseOrder(symbol, direction, offsetFlag, placeOrderCtx));
	if(pInputOrder.get() == NULL)
		return boost::make_tuple(false, string("Failed to build close order"));

	CManualSgOrderPlacer* placer = m_sgOrderStateMachine.CreateManualPlacer(pInputOrder, 2, this);
	
	bool succ = placer->DoAndWait();

	string errMsg;
	GB2312ToUTF_8(errMsg, placer->GetError().c_str());

	string pId = placer->Id();
	m_sgOrderStateMachine.RemovePlacer(pId);

	return boost::make_tuple(succ, errMsg);
}

bool COrderProcessor2::QuerySymbol( const std::string& symbol, entity::Quote** ppQuote )
{
	return m_pTradeAgent->QuerySymbol(symbol, ppQuote);
}
