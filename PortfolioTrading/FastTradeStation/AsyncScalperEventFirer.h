#pragma once

#include <string>

#include <boost/thread.hpp>
#include <boost/asio/steady_timer.hpp>

using namespace std;

class COrderProcessor2;
class CLeg;

class CAsyncScalperEventFirer
{
public:
	CAsyncScalperEventFirer(COrderProcessor2* pOrdProc, const string& ordRef):
	  m_pOrdProc(pOrdProc), m_orderRef(ordRef)
	{}
	virtual ~CAsyncScalperEventFirer(void){}

	virtual void Run(){}
	virtual void Cancel(){}

protected:

	COrderProcessor2* m_pOrdProc;
	string m_orderRef;
};

class CAsyncOpenOrderTimer : public CAsyncScalperEventFirer
{
public:
	CAsyncOpenOrderTimer(COrderProcessor2* pOrdProc, const string& ordRef, int expireMilliseconds):
	CAsyncScalperEventFirer(pOrdProc, ordRef),
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

class CAsyncNextQuoteWaiter : public CAsyncScalperEventFirer
{
public:
	CAsyncNextQuoteWaiter(COrderProcessor2* pOrdProc, const string& ordRef, CLeg* pLeg, long quoteTimestamp):
	CAsyncScalperEventFirer(pOrdProc, ordRef), m_pLeg(pLeg), m_quoteTimestamp(quoteTimestamp)
	, m_canceled(false)
	{
	}

	virtual void Run()
	{
		boost::thread th(boost::bind(&CAsyncNextQuoteWaiter::GetUpdateQuote, this));
	}

	virtual void Cancel(){ m_canceled = true; }

private:
	void GetUpdateQuote();
	void FireEvent(double last, double ask, double bid);

	CLeg* m_pLeg;
	long m_quoteTimestamp;
	bool m_canceled;
};

