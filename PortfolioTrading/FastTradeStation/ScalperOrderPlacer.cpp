#include "ScalperOrderPlacer.h"
#include "OrderProcessor2.h"
#include "SgOrderStateMachine.h"
#include "globalmembers.h"
#include "ScalperStrategy.h"

#include <boost/format.hpp>
#include <boost/date_time.hpp>

//void CScalperOrderPlacer::ModifyOrderPrice()
//{
//	trade::TradeDirectionType direction = m_pInputOrder->direction();
//	double basePx = 0;
//	if(m_closeMethod == entity::BASED_ON_NEXT_QUOTE)
//	{
//		if(direction == trade::BUY)
//		{
//			basePx = m_nextBid;
//		}
//		else
//			basePx = m_nextAsk;
//		
//		LOG_DEBUG(logger, boost::str(boost::format("In coming new quote's %s : %f") 
//			% (direction == trade::BUY ? "Bid" : "Ask") % basePx));
//	}
//	else
//	{
//		basePx = m_pInputOrder->limitprice();
//		LOG_DEBUG(logger, boost::str(boost::format("Last order limit price: %f") % basePx));
//	}
//
//	if(direction == trade::BUY)
//	{
//		double buy = basePx + m_precedence;
//		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Buy @ %f")
//			% Symbol() % buy));
//		m_pInputOrder->set_limitprice(buy);
//	}
//	else if(direction == trade::SELL)
//	{
//		double sell = basePx - m_precedence;
//		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Sell @ %f")
//			% Symbol() % sell));
//		m_pInputOrder->set_limitprice(sell);
//	}
//}

CScalperOrderPlacer::CScalperOrderPlacer( CSgOrderStateMachine* pStateMachine, CPortfolio* pPortfolio, trade::MultiLegOrder* pMultiLegOrder, const InputOrderPtr& inputOrder, int maxRetryTimes, COrderProcessor2* pOrderProc ) :
CSgOrderPlacer(pStateMachine, pPortfolio, pMultiLegOrder, 
	inputOrder, maxRetryTimes, true, pOrderProc),
	m_nextLast(0), m_nextAsk(0), m_nextBid(0)
{
	CScalperStrategy* pScalperStrategy = dynamic_cast<CScalperStrategy*>(pPortfolio->Strategy());
	if(pScalperStrategy != NULL)
	{
		m_precedence = pScalperStrategy->PriceTick();
		m_closeMethod = pScalperStrategy->CloseMethod();
	}
	else
	{
		m_precedence = 0.2;	// precedence so far only support IFxxxx
		m_closeMethod = entity::BASED_ON_NEXT_QUOTE;
	}
}

void CScalperOrderPlacer::OnOrderAccept( trade::Order* pOrd )
{
	boost::unique_lock<boost::mutex> lock(m_mut);
	m_pendingOrder = boost::shared_ptr<trade::Order>(new trade::Order);
	if(pOrd != NULL)
	{
		// back up pending order
		m_pendingOrder->CopyFrom(*pOrd);
		m_condCancelingWaitPendingOrder.notify_one();		
		CSgOrderPlacer::OnPending(pOrd);
	}
}

void CScalperOrderPlacer::OnPending( trade::Order* pOrd )
{
	boost::unique_lock<boost::mutex> lock(m_mut);
	if(!IsPendingOrderReady())
	{
		m_pendingOrder = boost::shared_ptr<trade::Order>(new trade::Order);
		if(pOrd != NULL)
		{
			// back up pending order
			m_pendingOrder->CopyFrom(*pOrd);
			m_condCancelingWaitPendingOrder.notify_one();		
			CSgOrderPlacer::OnPending(pOrd);
		}
	}
}

void CScalperOrderPlacer::OnCanceling(COrderEvent* transEvent)
{
	if(transEvent != NULL)
	{
		NextQuoteInEvent* pNextQuoteEvent = dynamic_cast<NextQuoteInEvent*>(transEvent);
		if(pNextQuoteEvent != NULL)
		{
			m_nextLast = pNextQuoteEvent->Last();
			m_nextAsk = pNextQuoteEvent->Ask();
			m_nextBid = pNextQuoteEvent->Bid();
		}
	}

	if(m_pendingOrder.get() != NULL)
		CancelOrder(m_pendingOrder.get());
	else
	{
		// has to wait pending order come out in a thread
		m_thWaitPending = boost::thread(boost::bind(&CScalperOrderPlacer::WaitForPendingOrderProc, this));
	}
}


void CScalperOrderPlacer::WaitForPendingOrderProc()
{
	boost::unique_lock<boost::mutex> lock(m_mut);
	if(m_condCancelingWaitPendingOrder.timed_wait(lock, boost::posix_time::seconds(1),
		boost::bind(&CScalperOrderPlacer::IsPendingOrderReady, this)))
	{
		CancelOrder(m_pendingOrder.get());
	}
	else
		logger.Warning("Pending order is empty when scalper order placer trying to cancel order");
}

void CScalperOrderPlacer::OnSubmittingOrder()
{
	if(IsOpenOrder())
	{
		int openTimeout = m_pPortf->OpenPendingTimeout();
		m_openOrderTimer = boost::shared_ptr<CAsyncOpenOrderTimer>(
					new CAsyncOpenOrderTimer(m_pOrderProcessor, m_currentOrdRef, openTimeout));
		m_openOrderTimer->Run();
	}
	else
	{
		m_nextQuoteWaiter = boost::shared_ptr<CAsyncOpenOrderTimer>(
			new CAsyncOpenOrderTimer(m_pOrderProcessor, m_currentOrdRef, 450));

		//CLeg* pLeg = m_pPortf->GetLeg(Symbol());
		//long quoteTimestamp = QuoteTimestamp();
		//assert(quoteTimestamp > 0);
		//m_nextQuoteWaiter = boost::shared_ptr<CAsyncNextQuoteWaiter>(
		//		new CAsyncNextQuoteWaiter(m_pOrderProcessor, m_currentOrdRef, pLeg, quoteTimestamp));
		m_nextQuoteWaiter->Run();
	}
}

bool CScalperOrderPlacer::IsOpenOrder()
{
	const string offsetStr = m_pInputOrder->comboffsetflag();
	char offsetFlag = offsetStr[0];
	if(offsetFlag > trade::OF_OPEN)
		return false;

	return true;
}

CScalperOrderPlacer::~CScalperOrderPlacer()
{
	{
		boost::unique_lock<boost::mutex> lock(m_mut);
		m_pendingOrder.reset();
		m_condCancelingWaitPendingOrder.notify_one();
	}
	
	m_thWaitPending.join();
}

void CScalperOrderPlacer::OnOrderComplete( trade::Order* pOrd )
{
	if(IsOpenOrder())
	{
		m_openOrderTimer->Cancel();
	}
	else
		m_nextQuoteWaiter->Cancel();
}

