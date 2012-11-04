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
m_isDone(false),
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
	trade::OrderStatusType status = pOrder->orderstatus();
	if(status == trade::ALL_TRADED)
	{
		// if order filled
		m_isDone = true;
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
		m_isDone = false;
	}
	// if insert failed
}

void COrderResubmitter::UpdateQuote( entity::Quote* pQuote )
{
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

	if(priceOutOfRange)
	{
		// order status is not completed
		CancelPending(m_lastRtnOrder);
	}
}

void COrderResubmitter::CancelPending(trade::Order* pOrder)
{
	m_pOrderProc->CancelOrder(pOrder->orderref(), pOrder->exchangeid(), pOrder->ordersysid(),
		pOrder->investorid(), pOrder->instrumentid());
}

void COrderResubmitter::ModifyOrder(double limitPrice)
{
	m_pOrderProc->ModifyOrder(m_mlOrderId, m_pendingOrder->orderref(), limitPrice);
}
