#pragma once

#include <string>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/asio/steady_timer.hpp>

using namespace std;

class CPortfolioOrderPlacer;
class CLegOrderPlacer;

class CAsyncOrderPendingTimer
{
public:
	CAsyncOrderPendingTimer(CPortfolioOrderPlacer* pOrdPlacer)
		: m_pOrdPlacer(pOrdPlacer)
		, m_legPlacer(NULL)
		, m_timer(m_io)
		, m_isStop(true)
	{
	}

	~CAsyncOrderPendingTimer()
	{
		m_thWaiting.join();
	}

	void SetLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer) { m_legPlacer = pLegOrderPlacer; }
	
	void Run(boost::chrono::steady_clock::time_point expireTimePoint)
	{
		if(IsStop())
		{
			m_isStop.store(false, boost::memory_order_relaxed);
			m_timer.expires_at(expireTimePoint);
			m_timer.async_wait(boost::bind(&CAsyncOrderPendingTimer::FireEvent, this, _1));
			boost::thread th(boost::bind(&boost::asio::io_service::run, &m_io));
			m_thWaiting.swap(th);
		}
	}

	void Cancel() 
	{ 
		if(m_isStop.exchange(true, boost::memory_order_acquire) == false)
		{
			m_timer.cancel();
			m_thWaiting.join();
		}
		
	}
	bool IsStop() { return m_isStop.load(boost::memory_order_consume); }

private:
	void FireEvent(const boost::system::error_code& e);

	boost::asio::io_service m_io;
	boost::asio::steady_timer m_timer;
	boost::atomic<bool> m_isStop;
	boost::thread m_thWaiting;

	CPortfolioOrderPlacer* m_pOrdPlacer;
	CLegOrderPlacer* m_legPlacer;
};

