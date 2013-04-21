#include "LegOrderPlacer.h"
#include "PortfolioOrderPlacer.h"
#include "globalmembers.h"

#include <boost/format.hpp>

#define MAX_CLOSE_TIMEOUT 490

CLegOrderPlacer::CLegOrderPlacer(CPortfolioOrderPlacer* portfOrdPlacer, int openTimeout, int maxRetry)
	: m_portfOrderPlacer(portfOrdPlacer)
	, m_pendingTimer(portfOrdPlacer)
	, m_maxRetry(maxRetry)
	, m_submitTimes(0)
	, m_seqenceNo(-1)
	, m_legIdx(-1)
	, m_openTimeout(openTimeout)
	, m_isPending(false)
	, m_bOrderReady(false)
	, m_isReadyForNextQuote(false)
{
	m_pendingTimer.SetLegOrderPlacer(this);
}


CLegOrderPlacer::~CLegOrderPlacer(void)
{
}

bool CLegOrderPlacer::IsOpen()
{
	assert(m_inputOrder.OffsetFlag().length() > 0);
	return m_inputOrder.OffsetFlag()[0] == trade::OF_OPEN;
}

void CLegOrderPlacer::OrderRef( const string& orderRef )
{
	m_inputOrder.set_orderref(orderRef);
}

const string& CLegOrderPlacer::OrderRef()
{
	return m_inputOrder.OrderRef();
}

void CLegOrderPlacer::StartPending(trade::Order* pendingOrder)
{
	m_exchId = pendingOrder->exchangeid();
	m_ordSysId = pendingOrder->ordersysid(); 
	m_userId = pendingOrder->userid();
	m_isPending = true;
	if(IsOpen())
	{
		m_pendingTimer.Run(boost::chrono::steady_clock::now() + boost::chrono::milliseconds(m_openTimeout));	
	}
	else
	{
		m_pendingTimer.Run(m_portfOrderPlacer->TrigTimestamp() + boost::chrono::milliseconds(MAX_CLOSE_TIMEOUT));
	}
}

void CLegOrderPlacer::Reset()
{
	m_submitTimes = 0;
	
	m_isPending = false;
	m_exchId.clear();
	m_ordSysId.clear();
	m_userId.clear();
}

void CLegOrderPlacer::ModifyPrice( entity::Quote* pQuote )
{
	m_isPending = false;
	m_exchId.clear();
	m_ordSysId.clear();
	m_userId.clear();

	LOG_DEBUG(logger, boost::str(boost::format("Next quote (%s) - L:%.2f, A:%.2f, B:%.2f")
		% pQuote->symbol() % pQuote->last() % pQuote->ask() % pQuote->bid()));

	trade::TradeDirectionType direction = m_inputOrder.Direction();
	double basePx = 0;
	if(direction == trade::BUY)
	{
		basePx = pQuote->bid();
	}
	else
		basePx = pQuote->ask();

	LOG_DEBUG(logger, boost::str(boost::format("In coming new quote's %s : %f") 
		% (direction == trade::BUY ? "Bid" : "Ask") % basePx));

	if(direction == trade::BUY)
	{
		double buy = basePx + m_priceTick;
		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Buy @ %f")
			% Symbol() % buy));
		m_inputOrder.set_limitprice(buy);
	}
	else if(direction == trade::SELL)
	{
		double sell = basePx - m_priceTick;
		LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Sell @ %f")
			% Symbol() % sell));
		m_inputOrder.set_limitprice(sell);
	}
}

void CLegOrderPlacer::UpdateOrder( const trade::Order& order )
{
	m_legOrder.CopyFrom(order);
	m_bOrderReady = true;
}

bool CLegOrderPlacer::WaitForNextQuote()
{
	if(!IsOpen()) 
		m_isReadyForNextQuote = true; 
	return m_isReadyForNextQuote;
}

void CLegOrderPlacer::CancelPending()
{
	m_pendingTimer.Cancel();
}
