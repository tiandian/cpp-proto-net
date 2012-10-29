#include "StdAfx.h"
#include "OrderProcessor.h"
#include "PortfolioOrderHelper.h"
#include "globalmembers.h"
#include "charsetconvert.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>

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
			mOrderId, vecInputOrders, 
			boost::bind(&COrderProcessor::SubmitOrderToTradeAgent, this, _1, _2)));

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

void COrderProcessor::CancelOrder( const string& orderId )
{

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

trade::Order* GetOrderByRef(trade::MultiLegOrder* mlOrder, const string& ordRef)
{
	trade::Order* pOrd = NULL;
	int count = mlOrder->legs_size();
	google::protobuf::RepeatedPtrField<trade::Order>* legs = mlOrder->mutable_legs();
	for(int i = 0; i < count; ++i)
	{
		pOrd = legs->Mutable(i);
		if(pOrd->orderref() == ordRef)
		{
			// find it
			break;
		}
	}
	return pOrd;
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
		const string& mlOrderId = iterTicket->second;
		
		OrderSenderMapIter iterOrdSender = m_orderSenderMap.find(mlOrderId);
		if(iterOrdSender != m_orderSenderMap.end())
		{
			if(IsTicketDone(*order))
			{
				if(IsTicketTraded(*order))
				{
					iterOrdSender->second->OrderDone(true);
				}
				else
				{
					iterOrdSender->second->OrderDone(false);

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
		}

		MultiLegOrderIter iterOrd = m_pendingMultiLegOrders.find(mlOrderId);
		if(iterOrd != m_pendingMultiLegOrders.end())
		{
			const MultiLegOrderPtr& mlOrder = iterOrd->second;
			trade::Order* pOrd = GetOrderByRef(mlOrder.get(), ordRef);
			
			pOrd->CopyFrom(*order);
			
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
			iterOrdSender->second->OrderDone(false);

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


