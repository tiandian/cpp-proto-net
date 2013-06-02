#pragma once

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/atomic.hpp>

class CThostFtdcMdApi;

class CMarketLoginWaiter
{
public:
	CMarketLoginWaiter(CThostFtdcMdApi* userApi) 
		: _userApi(userApi), _timer(_io_service), _stop(true){}
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
	CThostFtdcMdApi* _userApi;
	boost::atomic<bool> _stop;
};