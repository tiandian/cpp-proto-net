#include "StdAfx.h"
#include "OrderProcessor.h"
#include "globalmembers.h"
#include "charsetconvert.h"
#include "orderstatushelper.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>

trade::Order* GetOrderByRef(trade::MultiLegOrder* mlOrder, const string& ordRef)
{
	trade::Order* pOrdFound = NULL;
	int count = mlOrder->legs_size();
	google::protobuf::RepeatedPtrField<trade::Order>* legs = mlOrder->mutable_legs();
	for(int i = 0; i < count; ++i)
	{
		trade::Order* pOrd = legs->Mutable(i);
		if(pOrd->orderref() == ordRef)
		{
			// find it
			pOrdFound = pOrd;
			break;
		}
	}
	return pOrdFound;
}

bool IsOrderRejected(trade::Order* pOrder)
{
	return pOrder->ordersubmitstatus() > trade::ACCEPTED;
}

bool IsOrderPending(trade::Order* pOrder)
{
	trade::OrderStatusType status = pOrder->orderstatus();
	if(status == trade::NO_TRADE_QUEUEING ||
		status == trade::NO_TRADE_NOT_QUEUEING)
		return true;
	else
		return false;
}

bool IsTicketTraded(const trade::Order& order)
{
	trade::OrderStatusType status = order.orderstatus();
	return status == trade::ALL_TRADED;
}

bool IsTicketDone(const trade::Order& order)
{
	trade::OrderSubmitStatusType submitStatus = order.ordersubmitstatus();
	trade::OrderStatusType status = order.orderstatus();
	return submitStatus > trade::ACCEPTED || 
		status == trade::ALL_TRADED ||
		status == trade::ORDER_CANCELED;
}

void PrintOrderStatus(trade::Order* order)
{
	trade::OrderSubmitStatusType submitStatus = order->ordersubmitstatus();
	trade::OrderStatusType status = order->orderstatus();
	logger.Debug(boost::str(boost::format("Order(%s) - submit status(%s), order status(%s)")
		% order->orderref().c_str() % GetSumbitStatusText(submitStatus) % GetStatusText(status)));
}

bool IsMultiLegOrderDone(trade::MultiLegOrder* mlOrder)
{
	for(int i = 0; i < mlOrder->legs_size(); ++i )
	{
		const trade::Order leg = mlOrder->legs(i);
		if(!IsTicketDone(leg))
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// make sure give preferred order prior order ref
int COrderProcessor::IncrementalOrderRef(trade::MultiLegOrder* pMlOrder, int maxOrderRef )
{
	boost::mutex::scoped_lock lock(m_mutOrdRefIncr);
	std::vector<trade::Order*> tmpOrderVec;
	int count = pMlOrder->legs_size();
	for(int i = 0; i < count; ++i)
	{
		trade::Order* pOrd = pMlOrder->mutable_legs(i);
		if(pOrd->preferred())
			tmpOrderVec.insert(tmpOrderVec.begin(), pOrd);
		else
			tmpOrderVec.push_back(pOrd);
	}

	for(std::vector<trade::Order*>::iterator iter = tmpOrderVec.begin();
		iter != tmpOrderVec.end(); ++iter)
	{
		static char orderRef[10];
		sprintf_s(orderRef, "%d", maxOrderRef);
		(*iter)->set_orderref(orderRef);

		++maxOrderRef;
	}

	return maxOrderRef;
}

int COrderProcessor::IncrementalOrderRef(trade::Order* pLegOrd, int maxOrderRef)
{
	boost::mutex::scoped_lock lock(m_mutOrdRefIncr);
	static char orderRef[10];
	sprintf_s(orderRef, "%d", maxOrderRef);
	pLegOrd->set_orderref(orderRef);

	return ++maxOrderRef;
}

int COrderProcessor::IncrementalOrderRef(trade::InputOrder* pInputOrd, int maxOrderRef)
{
	boost::mutex::scoped_lock lock(m_mutOrdRefIncr);
	static char orderRef[10];
	sprintf_s(orderRef, "%d", maxOrderRef);
	pInputOrd->set_orderref(orderRef);

	return ++maxOrderRef;
}


COrderProcessor::COrderProcessor(void):
m_pTradeAgent(NULL),
m_maxOrderRef(0)
{
}

COrderProcessor::~COrderProcessor(void)
{
}

void COrderProcessor::SubmitOrder( MultiLegOrderPtr multilegOrder )
{
	boost::mutex::scoped_lock lock(m_mutTicketOrderMap);

	int count = m_pendingMultiLegOrders.size();
	const string& mOrderId = multilegOrder->orderid();
	
	m_maxOrderRef = IncrementalOrderRef(multilegOrder.get(), m_maxOrderRef);

	m_pendingMultiLegOrders.insert(make_pair(mOrderId, multilegOrder));

	std::vector<boost::shared_ptr<trade::InputOrder>> vecInputOrders;
	int ordCount = GetInputOrders(multilegOrder.get(), &vecInputOrders);
	
	BOOST_FOREACH(const boost::shared_ptr<trade::InputOrder>& iOrd, vecInputOrders)
	{
		m_pTradeAgent->SubmitOrder(iOrd.get());
		m_pendingTicketOrderMap.insert(make_pair(iOrd->orderref(), multilegOrder->orderid()));
	}

	// Publish order sent
	PublishMultiLegOrderUpdate(multilegOrder.get());
}

void COrderProcessor::SubmitOrderToTradeAgent(trade::InputOrder* pOrder, const string& mlOrderId)
{
	boost::mutex::scoped_lock lock(m_mutTicketOrderMap);
	logger.Trace(boost::str(boost::format("Truly sumbit order(%s - %s) to trade agent") 
		% mlOrderId.c_str() % pOrder->orderref().c_str()));
	m_pTradeAgent->SubmitOrder(pOrder);
	m_pendingTicketOrderMap.insert(make_pair(pOrder->orderref(), mlOrderId));
}

void COrderProcessor::SubmitOrder2(MultiLegOrderPtr multilegOrder)
{
	CSequenceOrderSender* pOrderSender = NULL;
	
	{
		boost::mutex::scoped_lock lock(m_mutTicketOrderMap);

		int count = m_pendingMultiLegOrders.size();
		const string& mOrderId = multilegOrder->orderid();

		m_maxOrderRef = IncrementalOrderRef(multilegOrder.get(), m_maxOrderRef);

		m_pendingMultiLegOrders.insert(make_pair(mOrderId, multilegOrder));

		InputOrderVectorPtr vecInputOrders(new InputOrderVector);
		int ordCount = GetInputOrders(multilegOrder.get(), vecInputOrders);

		OrderSenderPtr orderSender(
			new CSequenceOrderSender(
			multilegOrder->portfolioid(), mOrderId, vecInputOrders, this));

		m_orderSenderMap.insert(make_pair(mOrderId, orderSender));
		pOrderSender = orderSender.get();
	}

	pOrderSender->Start();

	PublishMultiLegOrderUpdate(multilegOrder.get());
}

//////////////////////////////////////////////////////////////////////////
// if given order is preferred order, set submit status INSERT_REJECTED, meanwhile
// set other order (not submit) status INSERT_REJECTED as well.
// if given order is NOT preferred order, set submit status INSERT_REJECTED as normal
// and do not touch other order's submit status as long as it is already 'submit' successfully
void COrderProcessor::SetNonPreferredOrderStatus(
	trade::MultiLegOrder* mlOrder, const string& prefOrdRef,
	trade::OrderStatusType otherStatus)
{
	int count = mlOrder->legs_size();
	google::protobuf::RepeatedPtrField<trade::Order>* legs = mlOrder->mutable_legs();
	for(int i = 0; i < count; ++i)
	{
		trade::Order* pOrd = legs->Mutable(i);
		if(pOrd->orderref() != prefOrdRef)
		{
			if(	pOrd->orderstatus() == trade::STATUS_UNKNOWN)
			{
				pOrd->set_orderstatus(otherStatus);
				PublishOrderUpdate(mlOrder->portfolioid(), mlOrder->orderid(), pOrd);
			}
		}
	}
}

void COrderProcessor::CancelOrder(	const std::string& ordRef, 
									const std::string& exchId, 
									const std::string& ordSysId, 
									const std::string& userId,
									const std::string& symbol)
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

void COrderProcessor::CancelOrder(const string& mlOrderId, const string& ordRef)
{
	MultiLegOrderIter iterOrd = m_pendingMultiLegOrders.find(mlOrderId);
	if(iterOrd != m_pendingMultiLegOrders.end())
	{
		const MultiLegOrderPtr& mlOrd = iterOrd->second;
		trade::Order* pOrder = GetOrderByRef(mlOrd.get(), ordRef);

		CancelOrder(pOrder->orderref(), pOrder->exchangeid(), pOrder->ordersysid(),
			pOrder->investorid(), pOrder->instrumentid());
	}
}

void COrderProcessor::ModifyOrder(const string& mlOrderId, const string& legOrderRef, double limitprice, string* modifiedOrdRef)
{
	// find multi leg order
	MultiLegOrderIter iterOrd = m_pendingMultiLegOrders.find(mlOrderId);
	if(iterOrd != m_pendingMultiLegOrders.end())
	{
		const MultiLegOrderPtr& mlOrd = iterOrd->second;
		
		// Modify order to be sent first 
		trade::Order* pOrd = GetOrderByRef(mlOrd.get(), legOrderRef);
		// remove old order ref from ticket order map
		m_pendingTicketOrderMap.erase(legOrderRef);
		// assign a new order ref
		m_maxOrderRef = IncrementalOrderRef(pOrd, m_maxOrderRef);
		// set new limit price
		pOrd->set_limitprice(limitprice);
		pOrd->set_ordersubmitstatus(trade::NOT_SUBMITTED);
		pOrd->set_orderstatus(trade::STATUS_UNKNOWN);

		*modifiedOrdRef = pOrd->orderref();

		// Then, has to update order ref for unsubmit order
		int lCount = mlOrd->legs_size();
		for(int i = 0; i < lCount; ++i)
		{
			trade::Order* legOrd = mlOrd->mutable_legs(i);
			if(legOrd->orderref() != *modifiedOrdRef &&
				legOrd->ordersubmitstatus() == trade::NOT_SUBMITTED)
			{
				m_maxOrderRef = IncrementalOrderRef(legOrd, m_maxOrderRef);
			}
		}
	}
}

void COrderProcessor::Initialize( CTradeAgent* pTradeAgent )
{
	m_pTradeAgent = pTradeAgent;
	if(m_pTradeAgent != NULL)
		m_pTradeAgent->SetCallbackHanlder(this);
}

void COrderProcessor::OnRspUserLogin( bool succ, std::string& msg, int initOrderRefID )
{
	if(succ)
		m_maxOrderRef = initOrderRefID;
	else
	{
		string warn = "Trade login failed due to " + msg;
		logger.Warning(warn);
	}
}

void COrderProcessor::RemoveFromPending( trade::MultiLegOrder* pMlOrder )
{
	// remove from ticket order map
	for(int i = 0; i < pMlOrder->legs_size(); ++i )
	{
		const trade::Order leg = pMlOrder->legs(i);
		const string& ordRef = leg.orderref();
		m_pendingTicketOrderMap.erase(ordRef);
	}

	const string& mlOrderId = pMlOrder->orderid();
	
	// remove from orderSender map if using sequence order sender
	m_orderSenderMap.erase(mlOrderId);

	// remove from pending mulite leg orders list
	m_pendingMultiLegOrders.erase(mlOrderId);
}

// Order submit succeeded
void COrderProcessor::OnRtnOrder( trade::Order* order )
{
	boost::mutex::scoped_lock lock(m_mutTicketOrderMap);
	const string& ordRef = order->orderref();
	PendingTktOrdMapIter iterTicket = m_pendingTicketOrderMap.find(ordRef);
	if(iterTicket != m_pendingTicketOrderMap.end())
	{
		string mlOrderId = iterTicket->second;
		
		OrderSenderMapIter iterOrdSender = m_orderSenderMap.find(mlOrderId);
		if(iterOrdSender != m_orderSenderMap.end())
		{
			if(IsTicketDone(*order))
			{
				bool stopDueToPreferredNotFilled = iterOrdSender->second->CheckOrderStatus(order);
				if(stopDueToPreferredNotFilled)
				{
					logger.Trace(boost::str(boost::format("MultiLegOrder(%s) has stop due to preferred order not being filled")
						% mlOrderId.c_str()));
					// find multi leg order
					MultiLegOrderIter iterOrd = m_pendingMultiLegOrders.find(mlOrderId);
					if(iterOrd != m_pendingMultiLegOrders.end())
					{
						// set preferred order insert_rejected, and set other order NOT started
						const MultiLegOrderPtr& mlOrder = iterOrd->second;
						SetNonPreferredOrderStatus(mlOrder.get(), ordRef, trade::ORDER_CANCELED); 
					}
				}
			}
			else if(IsOrderPending(order))
			{
				logger.Trace(boost::str(boost::format("Order(%s, %s) is pending")
					% order->instrumentid().c_str() % order->orderref().c_str()));
				iterOrdSender->second->OrderPending();
			}
		}

		MultiLegOrderIter iterOrd = m_pendingMultiLegOrders.find(mlOrderId);
		if(iterOrd != m_pendingMultiLegOrders.end())
		{
			const MultiLegOrderPtr& mlOrder = iterOrd->second;
			trade::Order* pOrd = GetOrderByRef(mlOrder.get(), ordRef);
			
			if(pOrd != NULL)
			{
				pOrd->CopyFrom(*order);
			}
			else
			{
				// Order already been cancelled and replaced with new order ref
				// push incoming order directly
				pOrd = order;
			}
			
			string ordStatusMsg;
			GB2312ToUTF_8(ordStatusMsg, order->statusmsg().c_str());
			pOrd->set_statusmsg(ordStatusMsg);

			// publish order updated
			PublishOrderUpdate(mlOrder->portfolioid(), mlOrder->orderid(), pOrd);

			if(IsMultiLegOrderDone(mlOrder.get()))
			{
				OnPortfolioPositionChanged(mlOrder);
				RemoveFromPending(mlOrder.get());
			}
		}
	}
	else	// manual placed order will be here
	{
		PrintOrderStatus(order);
		const string& ordRef = order->orderref();
		if(IsOrderRejected(order))
			m_placeOrderStateMachine.Transition(ordRef, RejectEvent(order));
		else if(IsTicketTraded(*order))
			m_placeOrderStateMachine.Transition(ordRef, CompleteEvent(order));
		else if(IsTicketDone(*order)) // order cancelled
			m_placeOrderStateMachine.Transition(ordRef, CancelSuccessEvent(order));
		else if(IsOrderPending(order))
			m_placeOrderStateMachine.Transition(ordRef, PendingEvent(order));
	}
}

// Order submit failed
void COrderProcessor::OnRspOrderInsert( bool succ, const std::string& orderRef, const std::string& msg )
{
	boost::mutex::scoped_lock lock(m_mutTicketOrderMap);
	PendingTktOrdMapIter iterTicket = m_pendingTicketOrderMap.find(orderRef);
	if(iterTicket != m_pendingTicketOrderMap.end())
	{
		const string& mlOrderId = iterTicket->second;

		OrderSenderMapIter iterOrdSender = m_orderSenderMap.find(mlOrderId);
		if(iterOrdSender != m_orderSenderMap.end())
		{
			iterOrdSender->second->OrderInsertFailed();

			// find multi leg order
			MultiLegOrderIter iterOrd = m_pendingMultiLegOrders.find(mlOrderId);
			if(iterOrd != m_pendingMultiLegOrders.end())
			{
				// set preferred order insert_rejected, and set other order NOT started
				const MultiLegOrderPtr& mlOrder = iterOrd->second;
				SetNonPreferredOrderStatus(mlOrder.get(), orderRef, trade::ORDER_CANCELED); 
			}
		}

		MultiLegOrderIter iterOrd = m_pendingMultiLegOrders.find(mlOrderId);
		if(iterOrd != m_pendingMultiLegOrders.end())
		{
			const MultiLegOrderPtr& mlOrder = iterOrd->second;
			trade::Order* pOrd = GetOrderByRef(mlOrder.get(), orderRef);

			pOrd->set_ordersubmitstatus(trade::INSERT_REJECTED);

			string ordStatusMsg;
			GB2312ToUTF_8(ordStatusMsg, msg.c_str());

			// set error message
			pOrd->set_statusmsg(ordStatusMsg);

			// publish order updated
			PublishOrderUpdate(mlOrder->portfolioid(), mlOrder->orderid(), pOrd);

			if(IsMultiLegOrderDone(mlOrder.get()))
			{
				RemoveFromPending(mlOrder.get());
			}
		}
	}
	else	// manual placed order will be here
	{
		string errorMsg;
		GB2312ToUTF_8(errorMsg, msg.c_str());
		m_placeOrderStateMachine.Transition(orderRef, SubmitFailedEvent(errorMsg));
	}
}

void COrderProcessor::OnRtnTrade( trade::Trade* pTrade )
{
	PublishTradeUpdate(pTrade);
}

void COrderProcessor::OnRspQryTradingAccount( const trade::AccountInfo& accountInfo )
{
	boost::mutex::scoped_lock lock(m_mutQryAcct);
	accountInfo.SerializeToString(&m_serializedQryAcctInfo);
	m_condQryAcct.notify_one();
}

bool COrderProcessor::QueryAccountInfo(string* outSerializedAcctInfo)
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

void COrderProcessor::OnRspOrderAction( bool succ, const std::string& orderRef, const std::string& msg )
{
	if(succ)
		logger.Info(boost::str(boost::format("Cancel order(%s) succeeded.") % orderRef));
	else
	{
		logger.Info(boost::str(boost::format("Cancel order(%s) failed. message: %s") 
								% orderRef.c_str() % msg.c_str()));

		m_placeOrderStateMachine.Transition(orderRef, CancelFailedEvent(NULL));
	}
}

void COrderProcessor::SetPushPortfolioFunc( PushMultiLegOrderFunc funcPushMLOrder )
{
	m_pushMultiOrdFunc = funcPushMLOrder;
}

void COrderProcessor::PublishMultiLegOrderUpdate( trade::MultiLegOrder* pOrder )
{
	if(!m_pushMultiOrdFunc.empty())
	{
		m_pushMultiOrdFunc(pOrder);
	}
}

void COrderProcessor::SetPushOrderFunc( PushLegOrderFunc funcPushOrder )
{
	m_pushLegOrderFunc = funcPushOrder;
}

void COrderProcessor::PublishOrderUpdate( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	if(!m_pushLegOrderFunc.empty())
	{
		m_pushLegOrderFunc(portfId, mlOrderId, legOrd);
	}
}

void COrderProcessor::SetPushTradeFunc( PushTradeFunc funcPushTrade )
{
	m_pushTradeFunc = funcPushTrade;
}

void COrderProcessor::PublishTradeUpdate( trade::Trade* pTrade )
{
	if(!m_pushTradeFunc.empty())
	{
		m_pushTradeFunc(pTrade);
	}
}

void COrderProcessor::SetPushPositionChangeFunc( PushPortfolioPositionChangeFunc funcPushPosiChange )
{
	m_pushPortfPosiChangeFunc = funcPushPosiChange;
}

void COrderProcessor::OnPortfolioPositionChanged( const MultiLegOrderPtr& multilegOrder )
{
	if(!m_pushPortfPosiChangeFunc.empty())
	{
		m_pushPortfPosiChangeFunc(multilegOrder);
		PublishMultiLegOrderUpdate(multilegOrder.get());
	}
}

void COrderProcessor::SetPushResubmitterChangeFunc( PushPortfolioResubmitterChangeFunc funcResubmitterChange )
{
	m_pushResubmitterChangeFunc = funcResubmitterChange;
}

void COrderProcessor::ChangePortfolioResubmitter( const string& portfId, COrderResubmitter* pResubmitter, bool isAdding )
{
	if(!m_pushResubmitterChangeFunc.empty())
	{
		m_pushResubmitterChangeFunc(portfId, pResubmitter, isAdding);
	}
}

void COrderProcessor::PublishPositionDetail( trade::PositionDetailInfo* pPosiDetailInfo )
{
	if(!m_pushPosiDetailFunc.empty())
	{
		m_pushPosiDetailFunc(pPosiDetailInfo);
	}
}

void COrderProcessor::SetPushPositionDetailFunc( PushPositionDetailFunc funcPushPosiDetail )
{
	m_pushPosiDetailFunc = funcPushPosiDetail;
}

void COrderProcessor::OnRspQryInvestorPositionDetail( trade::PositionDetailInfo* pPositionDetail )
{
	PublishPositionDetail(pPositionDetail);
}

void COrderProcessor::QueryPositionDetails( const string& symbol )
{
	m_pTradeAgent->QueryPositionDetails(symbol);
}

boost::tuple<bool, string> COrderProcessor::ManualCloseOrder( const string& symbol, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, PlaceOrderContext* placeOrderCtx )
{
	bool closeSucc = false;
	string errorMsg;
	
	entity::Quote* pQuote = NULL;
	bool succ = m_pTradeAgent->QuerySymbol(symbol, &pQuote);
	if(succ)
	{
		logger.Info(boost::str(boost::format("Query Quote %s: %d") 
			% symbol.c_str() % pQuote->last()));

		double limitPrice = direction == trade::SELL ? pQuote->bid() : pQuote->ask();

		boost::shared_ptr<trade::InputOrder> closeOrder(
			BuildCloseOrder(symbol, limitPrice, direction, offsetFlag, placeOrderCtx));

		m_maxOrderRef = IncrementalOrderRef(closeOrder.get(), m_maxOrderRef);

		m_pTradeAgent->SubmitOrder(closeOrder.get());

		delete pQuote;

		closeSucc = true;
	}
	else
	{
		closeSucc = false;
		errorMsg = "Cannot get symbol quote";
	}

	return boost::make_tuple(closeSucc, errorMsg);
}

trade::InputOrder* COrderProcessor::BuildSingleOrder(const string& symbol, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, PlaceOrderContext* placeOrderCtx)
{
	entity::Quote* pQuote = NULL;
	bool succ = m_pTradeAgent->QuerySymbol(symbol, &pQuote);
	if(succ)
	{
		logger.Info(boost::str(boost::format("Query Quote %s: %d") 
			% symbol.c_str() % pQuote->last()));

		//double limitPrice = direction == trade::SELL ? pQuote->bid() : pQuote->ask();
		double limitPrice = direction == trade::SELL ? pQuote->last() + 2 : pQuote->last() - 2;

		trade::InputOrder* closeOrder(
			BuildCloseOrder(symbol, limitPrice, direction, trade::OF_CLOSE_YESTERDAY, placeOrderCtx));

		m_maxOrderRef = IncrementalOrderRef(closeOrder, m_maxOrderRef);

		delete pQuote;

		return closeOrder;
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot query quote %s") % symbol));
	}
	return NULL;
}

boost::tuple<bool, string> COrderProcessor::PlaceOrder( const string& symbol, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, PlaceOrderContext* placeOrderCtx )
{
	/*entity::Quote* pQuote = NULL;
	bool succ = m_pTradeAgent->QuerySymbol(symbol, &pQuote);
	return boost::make_tuple(true, "");*/

	ManualOrderPlacerPtr placer = m_placeOrderStateMachine.CreatePlacer();
	placer->SetBuildOrderFunc(boost::bind(&COrderProcessor::BuildSingleOrder, this, symbol, direction, offsetFlag, placeOrderCtx));
	placer->SetSubmitOrderFunc(boost::bind(&CTradeAgent::SubmitOrder, m_pTradeAgent, _1));
	placer->SetCancelOrderFunc(boost::bind(&COrderProcessor::CancelOrder, this, _1, _2, _3, _4, _5));

	bool succ = placer->Do();

	return boost::make_tuple(succ, placer->GetError());
}


