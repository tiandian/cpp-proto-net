#include "StdAfx.h"
#include "OrderResubmitter.h"
#include "OrderProcessor.h"
#include "globalmembers.h"

#include <boost/format.hpp>

COrderResubmitter::COrderResubmitter(
	const string& mlOrderId,
	trade::InputOrder* pInputOrd,
	COrderProcessor* pOrderProc,
	int retryTimes):
m_mlOrderId(mlOrderId),
m_pendingOrder(pInputOrd),
m_pOrderProc(pOrderProc),
m_remainingRetry(retryTimes),
m_isDone(NotDone),
m_quoteAsk(0),
m_quoteBid(0),
m_bEverModifyOrder(false),
m_isCanceling(false),
m_isPending(false)
{
}


COrderResubmitter::~COrderResubmitter(void)
{
}

void COrderResubmitter::Start()
{
	m_sendingThread = boost::thread(boost::bind(&COrderProcessor::SubmitOrderToTradeAgent, m_pOrderProc, m_pendingOrder, m_mlOrderId));
}

void COrderResubmitter::OnOrderReturn( trade::Order* pOrder )
{
	boost::mutex::scoped_lock l(m_mut);

	trade::OrderStatusType status = pOrder->orderstatus();
	if(status == trade::ALL_TRADED)
	{
		logger.Trace(boost::str(boost::format("Target Order(%s) is completed")
			% pOrder->instrumentid().c_str()));
		// if order filled
		m_isDone = Filled;
	}
	else if(status == trade::ORDER_CANCELED)
	{
		// if order cancelled
		m_isCanceling = false;
		m_isPending = false;

		logger.Trace(boost::str(boost::format("Target Order(%s) is CANCELED. Retry - %d")
			% pOrder->instrumentid().c_str() % m_remainingRetry));
		if(m_remainingRetry > 0)
		{
			trade::TradeDirectionType direction = m_pendingOrder->direction();
			if(direction == trade::BUY)
			{
				logger.Trace(boost::str(boost::format("Modify order(%s): Buy @ %f")
					% pOrder->instrumentid().c_str() % m_quoteAsk));
				ModifyOrder(m_quoteAsk);
			}
			else if(direction == trade::SELL)
			{
				logger.Trace(boost::str(boost::format("Modify order(%s): Sell @ %f")
					% pOrder->instrumentid().c_str() % m_quoteBid));
				ModifyOrder(m_quoteBid);
			}
			--m_remainingRetry;
			Start();
		}
		else	// retry => 0
		{
			logger.Trace(boost::str(boost::format("Retry for order(%s) exhausts")
				% pOrder->instrumentid().c_str()));
			m_isDone = Canceled;
		}
	}
	// if insert failed
}

void COrderResubmitter::UpdateQuote( entity::Quote* pQuote )
{
	boost::mutex::scoped_lock l(m_mut);

	m_quoteAsk = pQuote->ask();
	m_quoteBid = pQuote->bid();

	if(!m_isPending || m_isCanceling) 
		return;

	bool priceOutOfRange = false;
	double limitPx = m_pendingOrder->limitprice();
	trade::TradeDirectionType direction = m_pendingOrder->direction();
	if(direction == trade::BUY)
	{
		logger.Trace(boost::str(boost::format("Buy: Ask(%f) ?> Lmt Px(%f)")
			% m_quoteAsk % limitPx));
		priceOutOfRange = m_quoteAsk > limitPx;
	}
	else if(direction == trade::SELL)
	{
		logger.Trace(boost::str(boost::format("Sell: Bid(%f) ?< Lmt Px(%f)")
			% m_quoteBid % limitPx));
		priceOutOfRange = m_quoteBid < limitPx;
	}
	else
		_ASSERT(false);

	if(priceOutOfRange)
	{
		// order status is not completed
		CancelPending();
	}
}

void COrderResubmitter::CancelPending()
{
	logger.Trace(boost::str(boost::format("Resubmitter CANCEL order(%s-%s)")
		% m_mlOrderId.c_str() % m_pendingOrder->orderref().c_str()));
	m_isCanceling = true;
	m_pOrderProc->CancelOrder(m_mlOrderId, m_pendingOrder->orderref());
}

void COrderResubmitter::ModifyOrder(double limitPrice)
{
	string newOrdRef;
	m_pOrderProc->ModifyOrder(m_mlOrderId, m_pendingOrder->orderref(), limitPrice, &newOrdRef);

	m_pendingOrder->set_orderref(newOrdRef);
	m_pendingOrder->set_limitprice(limitPrice);

	m_bEverModifyOrder = true;
}

void COrderResubmitter::OrderPending()
{
	m_isPending = true;
}
