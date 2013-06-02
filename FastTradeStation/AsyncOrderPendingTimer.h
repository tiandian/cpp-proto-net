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
	
	void Run(boost::chrono::steady_clock::time_point expireTimePoint);

	void Cancel() 
	{ 
		if(m_isStop.exchange(true, boost::memory_order_acquire) == false)
		{
			m_timer.cancel();
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

