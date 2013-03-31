#include "ScalperOrderPlacer.h"
#include "AsyncScalperEventFirer.h"
#include "OrderProcessor2.h"
#include "SgOrderStateMachine.h"
#include "globalmembers.h"
#include "ScalperStrategy.h"

#include <boost/format.hpp>

void CScalperOrderPlacer::ModifyOrderPrice()
{
	trade::TradeDirectionType direction = m_pInputOrder->direction();
	double basePx = 0;
	if(m_closeMethod == entity::BASED_ON_NEXT_QUOTE)
	{
		if(direction == trade::BUY)
		{
			basePx = m_nextBid;
		}
		else
			basePx = m_nextAsk;
		
		LOG_DEBUG(logger, boost::str(boost::format("In coming new quote's %s : %f") 
			% (direction == trade::BUY ? "Bid" : "Ask") % basePx));
	}
	else
	{
		basePx = m_pInputOrder->limitprice();
		LOG_DEBUG(logger, boost::str(boost::format("Last order limit price: %f") % basePx));
	}

	if(direction == trade::BUY)
	{
		double buy = basePx + m_precedence;
		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Buy @ %f")
			% Symbol() % buy));
		m_pInputOrder->set_limitprice(buy);
	}
	else if(direction == trade::SELL)
	{
		double sell = basePx - m_precedence;
		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Sell @ %f")
			% Symbol() % sell));
		m_pInputOrder->set_limitprice(sell);
	}
}

CScalperOrderPlacer::CScalperOrderPlacer( CSgOrderStateMachine* pStateMachine, CPortfolio* pPortfolio, trade::MultiLegOrder* pMultiLegOrder, const InputOrderPtr& inputOrder, int maxRetryTimes, COrderProcessor2* pOrderProc ) :
CSgOrderPlacer(pStateMachine, pPortfolio, pMultiLegOrder, 
	inputOrder, maxRetryTimes, true, pOrderProc), m_pendingOrder(NULL),
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

void CScalperOrderPlacer::OnPending( trade::Order* pOrd )
{
	m_pendingOrder = pOrd;
	CSgOrderPlacer::OnPending(pOrd);
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

	if(m_pendingOrder != NULL)
		CancelOrder(m_pendingOrder);
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
		CLeg* pLeg = m_pPortf->GetLeg(Symbol());
		
		m_nextQuoteWaiter = boost::shared_ptr<CAsyncNextQuoteWaiter>(
				new CAsyncNextQuoteWaiter(m_pOrderProcessor, m_currentOrdRef, pLeg, QuoteTimestamp()));
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

