#include "StdAfx.h"
#include "OrderProcessor2.h"
#include "globalmembers.h"
#include "orderhelper.h"

#include <boost/format.hpp>


COrderProcessor2::COrderProcessor2(void)
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

}

void COrderProcessor2::OnRspOrderInsert( bool succ, const std::string& orderRef, const std::string& msg )
{

}

void COrderProcessor2::OnRspOrderAction( bool succ, const std::string& orderRef, const std::string& msg )
{

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

}

void COrderProcessor2::OnRspQryInvestorPositionDetail( trade::PositionDetailInfo* pPositionDetail )
{

}

void COrderProcessor2::OnRspQryTradingAccount( const trade::AccountInfo& accountInfo )
{

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

}

COrderPlacer* COrderProcessor2::CreateSingleOrderPlacer(trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes)
{
	return m_sgOrderStateMachine.CreatePlacer(pMlOrder, pInputOrder, retryTimes, this);
}

void COrderProcessor2::RaiseMLOrderPlacerEvent( const string& mlOrdPlacerId, COrderEvent& orderEvent )
{
	m_mlOrderStateMachine.Transition(mlOrdPlacerId, orderEvent);
}

int COrderProcessor2::LockForSubmit( string& outOrdRef )
{
	int retOrderRef = -1;
	boost::unique_lock<boost::mutex> lock(m_mutOrdRefIncr);
	if(m_condSubmit.timed_wait(lock, boost::posix_time::seconds(3)))
	{
		retOrderRef = GenerateOrderRef(outOrdRef);
	}
	else
		logger.Warning("LOCK for submit order TIME OUT!");

	return retOrderRef;
}

bool COrderProcessor2::SubmitAndUnlock( trade::InputOrder* pOrder )
{
	boost::mutex::scoped_lock lock(m_mutOrdRefIncr);
	bool succ = SubmitOrderToTradeAgent(pOrder);
	m_condSubmit.notify_one();
	return succ;
}

int COrderProcessor2::GenerateOrderRef( string& outOrdRef )
{
	static char orderRef[10];
	sprintf_s(orderRef, "%d", m_maxOrderRef++);
	outOrdRef = orderRef;

	return m_maxOrderRef;
}

bool COrderProcessor2::SubmitOrderToTradeAgent( trade::InputOrder* pOrder )
{
	return true;
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
