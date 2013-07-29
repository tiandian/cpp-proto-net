#include "StdAfx.h"
#include "AsyncOrderPendingTimer.h"
#include "OrderProcessor.h"
#include "Portfolio.h"
#include "globalmembers.h"
#include "PortfolioOrderPlacer.h"

void CAsyncOrderPendingTimer::FireEvent( const boost::system::error_code& e )
{
	if(e.value() == boost::asio::error::operation_aborted)
	{
		m_isStop.store(true, boost::memory_order_release);
#ifdef LOG_FOR_TRADE
		LOG_INFO(logger, boost::str(boost::format("Pending Timer for order ref(%s) is canceled") 
			% m_legPlacer->OrderRef()));
#endif
		return; // timer canceled
	}
#ifdef LOG_FOR_TRADE
	if(m_legPlacer->IsOpen())
	{
		LOG_INFO(logger, boost::str(boost::format("Opening order ref(%s) Time up") % m_legPlacer->OrderRef()));
	}
	else
	{
		LOG_INFO(logger, boost::str(boost::format("Closing order ref(%s) Time up") % m_legPlacer->OrderRef()));
	}
#endif		
	m_pOrdPlacer->OnPendingTimeUp();
	m_isStop.store(true, boost::memory_order_release);
}

void CAsyncOrderPendingTimer::Run( boost::chrono::steady_clock::time_point expireTimePoint )
{
	if(IsStop())
	{
		m_io.reset();
		m_isStop.store(false, boost::memory_order_relaxed);
		m_timer.expires_at(expireTimePoint);
		m_timer.async_wait(boost::bind(&CAsyncOrderPendingTimer::FireEvent, this, _1));
		m_thWaiting = boost::thread(boost::bind(&boost::asio::io_service::run, &m_io));
#ifdef LOG_FOR_TRADE
		LOG_DEBUG(logger, boost::str(boost::format("Start pending timer for Order Ref:%s") % m_legPlacer->OrderRef()));
#endif
	}
	else
	{
		logger.Warning(boost::str(boost::format("Pending Timer(%s) Cannot start due to NOT STOPPED")
			% m_legPlacer->OrderRef()));
	}
}

