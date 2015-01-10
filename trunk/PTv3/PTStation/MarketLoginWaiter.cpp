#include "StdAfx.h"
#include "MarketLoginWaiter.h"

#ifndef USE_FEMAS_API
#include "QuoteProxy.h"
#else
#include "QuoteProxy_FM.h"
#endif

#include <iostream>

void CMarketLoginWaiter::Cancel()
{
	_timer.cancel();
}

void CMarketLoginWaiter::BeginWait(int secondsToWait)
{
	std::cout << "CMarketLoginWaiter(" << _quoteProxy->InvestorId() << "-" << _quoteProxy->ConnectingIP() << ") BEGIN ..." << std::endl;
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
		std::cout << "CMarketLoginWaiter(" << _quoteProxy->InvestorId() << "-" << _quoteProxy->ConnectingIP() << ") was CANCELED." << std::endl;
		return;
	}

	std::cout << "CMarketLoginWaiter(" << _quoteProxy->InvestorId() << "-" << _quoteProxy->ConnectingIP() << ") is Time up, Notify to Release" << std::endl;
	if(_quoteProxy != NULL)
		_quoteProxy->End();
}
