#include "StdAfx.h"
#include "OrderResubmitter.h"
#include "OrderProcessor.h"

#define INIT_RETRY_TIMES 2

COrderResubmitter::COrderResubmitter(
	const string& mlOrderId,
	trade::InputOrder* pInputOrd,
	COrderProcessor* pOrderProc):
m_mlOrderId(mlOrderId),
m_pendingOrder(pInputOrd),
m_pOrderProc(pOrderProc),
m_remainingRetry(INIT_RETRY_TIMES),
m_isDone(NotDone),
m_quoteAsk(0),
m_quoteBid(0)
{
}


COrderResubmitter::~COrderResubmitter(void)
{
}

void COrderResubmitter::Start()
{
	m_pOrderProc->SubmitOrderToTradeAgent(m_pendingOrder, m_mlOrderId);
}

void COrderResubmitter::OnOrderReturn( trade::Order* pOrder )
{
	boost::mutex::scoped_lock l(m_mut);

	trade::OrderStatusType status = pOrder->orderstatus();
	if(status == trade::ALL_TRADED)
	{
		// if order filled
		m_isDone = Filled;
	}
	else if(status == trade::ORDER_CANCELED)
	{
		// if order cancelled
		if(m_remainingRetry > 0)
		{
			trade::TradeDirectionType direction = m_pendingOrder->direction();
			if(direction == trade::BUY)
			{
				ModifyOrder(m_quoteAsk);
			}
			else if(direction == trade::SELL)
			{
				ModifyOrder(m_quoteBid);
			}
			--m_remainingRetry;
			Start();
		}
		m_isDone = Canceled;
	}
	// if insert failed
}

void COrderResubmitter::UpdateQuote( entity::Quote* pQuote )
{
	boost::mutex::scoped_lock l(m_mut);

	m_quoteAsk = pQuote->ask();
	m_quoteBid = pQuote->bid();

	bool priceOutOfRange = false;
	double limitPx = m_pendingOrder->limitprice();
	trade::TradeDirectionType direction = m_pendingOrder->direction();
	if(direction == trade::BUY)
	{
		priceOutOfRange = m_quoteAsk > limitPx;
	}
	else if(direction == trade::SELL)
	{
		priceOutOfRange = m_quoteBid < limitPx;
	}
	else
		_ASSERT(false);

	if(priceOutOfRange && !m_isDone)
	{
		// order status is not completed
		CancelPending();
	}
}

void COrderResubmitter::CancelPending()
{
	m_pOrderProc->CancelOrder(m_mlOrderId, m_pendingOrder->orderref());
}

void COrderResubmitter::ModifyOrder(double limitPrice)
{
	string newOrdRef;
	m_pOrderProc->ModifyOrder(m_mlOrderId, m_pendingOrder->orderref(), limitPrice, &newOrdRef);

	m_pendingOrder->set_orderref(newOrdRef);
	m_pendingOrder->set_limitprice(limitPrice);
}
