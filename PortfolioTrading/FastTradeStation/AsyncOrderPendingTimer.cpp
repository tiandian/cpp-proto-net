#include "AsyncOrderPendingTimer.h"
#include "OrderProcessor2.h"
#include "Portfolio.h"
#include "globalmembers.h"

#include <boost/format.hpp>

void CAsyncOrderPendingTimer::FireEvent( const boost::system::error_code& e )
{
	if(e.value() == boost::asio::error::operation_aborted)
	{
		m_isStop.store(true, boost::memory_order_release);
		LOG_INFO(logger, "Pending Timer is canceled");
		return; // timer canceled
	}

	if(m_legPlacer->IsOpen())
	{
		LOG_INFO(logger, boost::str(boost::format("Opening order ref(%s) Time up") % m_legPlacer->OrderRef()));
	}
	else
	{
		LOG_INFO(logger, boost::str(boost::format("Closing order ref(%s) Time up") % m_legPlacer->OrderRef()));
	}
		
	m_pOrdPlacer->OnPendingTimeUp();
	m_isStop.store(true, boost::memory_order_release);
}

void CAsyncOrderPendingTimer::Run( boost::chrono::steady_clock::time_point expireTimePoint )
{
	if(IsStop())
	{
		m_isStop.store(false, boost::memory_order_relaxed);
		m_timer.expires_at(expireTimePoint);
		m_timer.async_wait(boost::bind(&CAsyncOrderPendingTimer::FireEvent, this, _1));
		m_thWaiting = boost::thread(boost::bind(&boost::asio::io_service::run, &m_io));
	}
}

