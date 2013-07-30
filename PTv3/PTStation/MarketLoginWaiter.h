#pragma once

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/atomic.hpp>

class CQuoteProxy;

class CMarketLoginWaiter
{
public:
	CMarketLoginWaiter(CQuoteProxy* quoteProxy) 
		: _quoteProxy(quoteProxy), _timer(_io_service), _stop(true){}
	~CMarketLoginWaiter()
	{
		if(!_stop.load(boost::memory_order_consume))
			Cancel();

		_thread.join();
	}

	void Cancel();
	void BeginWait(int secondsToWait);

private:
	void TimeUp(const boost::system::error_code& e);

	boost::asio::io_service _io_service;
	boost::asio::steady_timer _timer;
	
	boost::thread _thread;
	CQuoteProxy* _quoteProxy;
	boost::atomic<bool> _stop;
};