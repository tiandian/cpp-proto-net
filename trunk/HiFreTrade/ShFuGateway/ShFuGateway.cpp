// ShFuGateway.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "ShFuGateway.h"
#include "LogManager.h"
#include "MarketAgent.h"
#include "OrderProcessor.h"
#include "TradeAgent.h"
#include "ClientAgent.h"
#include "Quote.h"

#include <boost/format.hpp>
#include <string>

extern CLogManager	logger;
CMarketAgent g_marketAgent;
COrderProcessor g_orderProcessor;
CTradeAgent g_tradeAgent;
CClientAgent g_clientAgent;

std::string _brokerID;
std::string _userID;
QuoteCallback _callbackHandler = NULL;

void Cleanup();
void SendQuoteUpdate(CQuote* pQuote);

SHFU_GATEWAY_EXPORT int __stdcall TestCall(int a, int b)
{
	int c = a + b;
	logger.Debug(boost::str(boost::format("Calculate result %d") % c));
	return c;
}

SHFU_GATEWAY_EXPORT bool __stdcall ConnectMarketAgent(	const char* brokerID, 
														const char* userID, 
														const char* password,
														QuoteCallback callbackHandler)
{
	_brokerID = brokerID;
	_userID = userID;

	logger.Debug(boost::str(boost::format("Login market with %s, %s, %s") % brokerID % userID % password));

	if(!g_marketAgent.Connect())
	{
		Cleanup();
		return false;
	}
	if(!g_marketAgent.Login(brokerID, userID, password))
	{
		Cleanup();
		return false;
	}

	g_orderProcessor.Initialize();
	g_clientAgent.Initialize(boost::bind(SendQuoteUpdate, _1));
	_callbackHandler = callbackHandler;

	return true;
}

SHFU_GATEWAY_EXPORT void __stdcall DisconnectMarketAgent()
{
	Cleanup();
}

SHFU_GATEWAY_EXPORT void __stdcall SetSymbol(const char* symbol)
{
	g_orderProcessor.SetSymbol(std::string(symbol));
}

SHFU_GATEWAY_EXPORT bool __stdcall Start(const BreakOutStrategy* pStrategy)
{
	return false;
}

SHFU_GATEWAY_EXPORT void __stdcall Stop()
{

}

void Cleanup()
{
	g_marketAgent.Logout(_brokerID.c_str(), _userID.c_str());
	g_marketAgent.Disconnect();
}

void SendQuoteUpdate(CQuote* pQuote)
{
	if(_callbackHandler != NULL)
	{
		QuoteData qd;
		strcpy(qd.caSymbol, pQuote->get_symbol().c_str());
		strcpy(qd.caTradingDay, pQuote->get_trading_day().c_str());
		qd.dLast = pQuote->get_last();
		qd.dPrevClose = pQuote->get_prev_close();
		qd.dOpen = pQuote->get_open();
		qd.dHigh = pQuote->get_high();
		qd.dLow = pQuote->get_low();
		qd.iVolume = pQuote->get_volume();
		qd.dTurnover = pQuote->get_turnover();
		qd.dClose = pQuote->get_close();
		strcpy(qd.caUpdateTime, pQuote->get_update_time().c_str());
		qd.iUpdateMillisec = pQuote->get_update_millisec();
		qd.dBid = pQuote->get_bid();
		qd.iBidSize = pQuote->get_bid_size();
		qd.dAsk = pQuote->get_ask();
		qd.iAskSize = pQuote->get_ask_size();
		_callbackHandler(&qd);
	}
}
