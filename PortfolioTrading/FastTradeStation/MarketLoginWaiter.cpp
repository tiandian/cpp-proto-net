#include "MarketLoginWaiter.h"
#include "ThostTraderApi/ThostFtdcMdApi.h"

#include <iostream>

void CMarketLoginWaiter::Cancel()
{
	_timer.cancel();
}

void CMarketLoginWaiter::BeginWait(int secondsToWait)
{
	_timer.expires_from_now(boost::chrono::seconds(secondsToWait));
	_timer.async_wait(boost::bind(&CMarketLoginWaiter::TimeUp, this, _1));
	_stop.store(false, boost::memory_order_release);
	_thread = boost::thread(boost::bind(&boost::asio::io_service::run, &_io_service));
}

void CMarketLoginWaiter::TimeUp( const boost::system::error_code& e )
{
	_stop.store(true, boost::memory_order_release);
	if(e.value() == boost::asio::error::operation_aborted)
	{
		std::cout << "CMarketLoginWaiter was CANCELED." << std::endl;
		return;
	}

	std::cout << "CMarketLoginWaiter is Time up, Notify to Release" << std::endl;
	if(_userApi != NULL)
		_userApi->Release();
}
