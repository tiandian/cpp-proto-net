#pragma once

#include <string>

#include <boost/thread.hpp>
#include <boost/asio/steady_timer.hpp>

using namespace std;

class CPortfolioOrderPlacer;
class CLeg;

class CAsyncScalperEventFirer
{
public:
	CAsyncScalperEventFirer(CPortfolioOrderPlacer* pOrdPlacer, const string& ordRef):
	  m_pOrdPlacer(pOrdPlacer), m_orderRef(ordRef)
	{}
	virtual ~CAsyncScalperEventFirer(void){}

	virtual void Run(){}
	virtual void Cancel(){}

protected:

	CPortfolioOrderPlacer* m_pOrdPlacer;
	string m_orderRef;
};

class CAsyncOpenOrderTimer : public CAsyncScalperEventFirer
{
public:
	CAsyncOpenOrderTimer(CPortfolioOrderPlacer* pOrdPlacer, const string& ordRef, int expireMilliseconds):
	CAsyncScalperEventFirer(pOrdPlacer, ordRef),
	m_timer(m_io, boost::chrono::milliseconds(expireMilliseconds))
	{
	}
	
	virtual void Run()
	{
		m_timer.async_wait(boost::bind(&CAsyncOpenOrderTimer::FireEvent, this, _1));
		boost::thread th(boost::bind(&boost::asio::io_service::run, &m_io));
	}
	virtual void Cancel() { m_timer.cancel(); }

private:
	void FireEvent(const boost::system::error_code& e);

	boost::asio::io_service m_io;
	boost::asio::steady_timer m_timer;
};

