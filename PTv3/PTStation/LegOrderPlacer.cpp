#include "StdAfx.h"
#include "LegOrderPlacer.h"
#include "PortfolioOrderPlacer.h"
#include "globalmembers.h"

#define MAX_CLOSE_TIMEOUT 490

CLegOrderPlacer::CLegOrderPlacer(CPortfolioOrderPlacer* portfOrdPlacer, int openTimeout, int maxRetry)
	: m_portfOrderPlacer(portfOrdPlacer)
	, m_pendingTimer(portfOrdPlacer)
	, m_maxRetry(maxRetry)
	, m_submitTimes(0)
	, m_sequenceNo(0)
	, m_legIdx(-1)
	, m_openTimeout(openTimeout)
	, m_isPending(false)
	, m_bOrderReady(false)
	, m_isPartiallyFilled(false)
	, m_priceTick(0)
	, m_modifyPriceWay(BASED_ON_TICK)
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

void CLegOrderPlacer::StartPending(const RtnOrderWrapperPtr& pendingOrder)
{
	m_exchId = pendingOrder->ExchangeId();
	m_ordSysId = pendingOrder->OrderSysId(); 
	m_userId = pendingOrder->UserId();
	m_isPending = true;
	if(IsOpen())
	{
		m_pendingTimer.Run(boost::chrono::steady_clock::now() + boost::chrono::milliseconds(m_openTimeout));	
	}
	/* Don't set timer for close order
	else
	{
		m_pendingTimer.Run(m_portfOrderPlacer->TrigTimestamp() + boost::chrono::milliseconds(MAX_CLOSE_TIMEOUT));
	}
	*/
}

void CLegOrderPlacer::Reset(bool afterCancel)
{
	if(!afterCancel)
	{
		m_submitTimes = 0;
		m_inputOrder.set_volumetotaloriginal(m_volumeOriginial);
	}
	
	m_isPending = false;
	m_bOrderReady = false;
	m_isPartiallyFilled = false;
	m_exchId.clear();
	m_ordSysId.clear();
	m_userId.clear();
	
	m_legOrder.set_volumetotaloriginal(0);
	m_legOrder.set_volumetraded(0);
	m_legOrder.set_ordersubmitstatus(trade::NOT_SUBMITTED);
	m_legOrder.set_orderstatus(trade::STATUS_UNKNOWN);
	m_legOrder.set_statusmsg("");
}

bool CLegOrderPlacer::ModifyPrice( entity::Quote* pQuote )
{
	if(m_modifyPriceWay == BASED_ON_TICK)
	{
		return ModifyPriceBasedOnTick(pQuote);
	}
	else
	{
		return ModifyPriceBasedOnOpposite(pQuote);
	}
}

bool CLegOrderPlacer::ModifyPriceBasedOnTick( entity::Quote* pQuote )
{
#ifdef LOG_FOR_TRADE
	LOG_DEBUG(logger, boost::str(boost::format("Next quote (%s) - L:%.2f, A:%.2f, B:%.2f")
		% pQuote->symbol() % pQuote->last() % pQuote->ask() % pQuote->bid()));
#endif
	trade::TradeDirectionType direction = m_inputOrder.Direction();
	double basePx = 0;
	if(direction == trade::BUY)
	{
		basePx = pQuote->bid();
	}
	else
		basePx = pQuote->ask();
#ifdef LOG_FOR_TRADE
	LOG_DEBUG(logger, boost::str(boost::format("In coming new quote's %s : %f") 
		% (direction == trade::BUY ? "Bid" : "Ask") % basePx));
#endif

	if(direction == trade::BUY)
	{
		double buy = basePx + m_priceTick;

#ifdef FAKE_DEAL
		bool needChange = true;
#else
		bool needChange = buy - m_inputOrder.LimitPrice() > 0.001;
#endif
		if(needChange)
		{
#ifdef LOG_FOR_TRADE
			LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Buy @ %f")
				% Symbol() % buy));
#endif
			m_inputOrder.set_limitprice(buy);
		}

		return needChange;
	}
	else if(direction == trade::SELL)
	{
		double sell = basePx - m_priceTick;
#ifdef FAKE_DEAL
		bool needChange = true;
#else
		bool needChange = m_inputOrder.LimitPrice() - sell  > 0.001;
#endif
		if(needChange)
		{
#ifdef LOG_FOR_TRADE
			LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Sell @ %f")
				% Symbol() % sell));
#endif
			m_inputOrder.set_limitprice(sell);
		}
		return needChange;
	}

	return true;
}

bool CLegOrderPlacer::ModifyPriceBasedOnOpposite( entity::Quote* pQuote )
{
	trade::TradeDirectionType direction = m_inputOrder.Direction();

	if(direction == trade::BUY)
	{
		double buy = pQuote->ask();

#ifdef FAKE_DEAL
		bool needChange = true;
#else
		bool needChange = buy - m_inputOrder.LimitPrice() > 0.001;
#endif
		if(needChange)
		{
#ifdef LOG_FOR_TRADE
			LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Buy @ %.2f -> %.2f")
				% Symbol() % m_inputOrder.LimitPrice() % buy));
#endif
			m_inputOrder.set_limitprice(buy);
		}

		return needChange;
	}
	else if(direction == trade::SELL)
	{
		double sell = pQuote->bid();
#ifdef FAKE_DEAL
		bool needChange = true;
#else
		bool needChange = m_inputOrder.LimitPrice() - sell  > 0.001;
#endif
		if(needChange)
		{
#ifdef LOG_FOR_TRADE
			LOG_DEBUG(logger, boost::str(boost::format("Modify order(%s): Sell @ %.2f -> %.2f")
				% Symbol() % m_inputOrder.LimitPrice() % sell));
#endif
			m_inputOrder.set_limitprice(sell);
		}
		return needChange;
	}

	return true;
}


void CLegOrderPlacer::UpdateOrder( const RtnOrderWrapperPtr& order )
{
	m_legOrderWrapper = order;
	m_bOrderReady = true;
}

void CLegOrderPlacer::CancelPending()
{
	m_pendingTimer.Cancel();
}

void CLegOrderPlacer::AdjustVolume( int adjustedVolume )
{
	m_inputOrder.set_volumetotaloriginal(adjustedVolume);
}

trade::Order& CLegOrderPlacer::OrderEntity()
{
	m_legOrderWrapper->ToEntity(&m_legOrder);
	return m_legOrder;
}

bool CArbitrageLegOrderPlacer::IsOpen()
{
	return m_sequenceNo == 0;
}
