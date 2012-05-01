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

	_callbackHandler = callbackHandler;

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

	//_callbackHandler = callbackHandler;
	g_clientAgent.SetQuoteCallback(boost::bind(SendQuoteUpdate, _1));

	return true;
}

SHFU_GATEWAY_EXPORT void __stdcall DisconnectMarketAgent()
{
	Cleanup();
}

SHFU_GATEWAY_EXPORT void __stdcall SetSymbol(const char* symbol)
{
	//g_orderProcessor.SetSymbol(std::string(symbol));
	if(_callbackHandler != NULL)
	{
		QuoteData qd;
		strcpy(qd.caSymbol, "test symbol");
		strcpy(qd.caTradingDay, "20120501");
		qd.dLast = 54321.6;
		qd.dPrevClose = 12345.6;
		qd.dOpen = 32456.1;
		qd.dHigh = 55555.5;
		qd.dLow = 11111.1;
		qd.iVolume = 666666;
		qd.dTurnover = 33330000;
		qd.dClose = 232.45;
		strcpy(qd.caUpdateTime, "11:20");
		qd.iUpdateMillisec = 2000;
		qd.dBid = 10;
		qd.iBidSize = 100;
		qd.dAsk = 20;
		qd.iAskSize = 200;
		_callbackHandler(&qd);
		//_callbackHandler("test symbol");
	}
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
		//_callbackHandler(pQuote->get_symbol().c_str());
	}
}
