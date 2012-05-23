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
#include "OperationRecordData.h"
#include "TimeNSalePacket.h"
#include "AccountInfoMsg.h"
#include "PositionDetailMsg.h"

#include <boost/format.hpp>
#include <string>

extern CLogManager	logger;
CMarketAgent g_marketAgent;
COrderProcessor g_orderProcessor;
CTradeAgent g_tradeAgent;
CClientAgent g_clientAgent;

QuoteCallback _quoteCallbackHandler = NULL;
OperationRecordCallback _recordCallbackHandler = NULL;
TimeNSalesCallback _tnsCallbackHandler = NULL;
AccountInfoCallback _acctCallbackHandler = NULL;
PositionDetailCallback _posiDetialCallbackHandler = NULL;

void Cleanup();
void SendQuoteUpdate(CQuote* pQuote);
void SendOperationRecords(COperationRecordData* pRecord);
void SendTimeNSales(CTimeNSalePacket* pTns);
void SendAccountInfo(CAccountInfoMsg* pAcctInfo);
void SendPositionDetail(CPositionDetailMsg* pPosiDetailMsg);

SHFU_GATEWAY_EXPORT int __stdcall TestCall(int a, int b)
{
	int c = a + b;
	logger.Debug(boost::str(boost::format("Calculate result %d") % c));

	//g_tradeAgent.QueryOrders();
	g_tradeAgent.QueryPositions();
	//g_tradeAgent.QueryPositionDetails();
	return c;
}

SHFU_GATEWAY_EXPORT bool __stdcall ConnectMarketAgent(	const char* brokerID, 
														const char* userID, 
														const char* password,
														QuoteCallback callbackHandler)
{
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
	g_clientAgent.SetQuoteCallback(boost::bind(SendQuoteUpdate, _1));
	_quoteCallbackHandler = callbackHandler;

	return true;
}

SHFU_GATEWAY_EXPORT void __stdcall DisconnectMarketAgent()
{
	Cleanup();
}

SHFU_GATEWAY_EXPORT bool __stdcall ConnectTradeAgent(	const char* brokerID, 
														const char* userID, 
														const char* password,
														OperationRecordCallback recordCallback,
														TimeNSalesCallback tnsCallback)
{
	logger.Debug(boost::str(boost::format("Login trade with %s, %s, %s") % brokerID % userID % password));

	g_tradeAgent.Initialize(&g_orderProcessor);

	if(!g_tradeAgent.Login(brokerID, userID, password))
	{
		Cleanup();
		return false;
	}

	g_clientAgent.SetOperationRecordCallback(boost::bind(SendOperationRecords, _1));
	_recordCallbackHandler = recordCallback;

	g_clientAgent.SetTimeNSalesCallback(boost::bind(SendTimeNSales, _1));
	_tnsCallbackHandler = tnsCallback;

	return true;
}

SHFU_GATEWAY_EXPORT void __stdcall DisconnectTradeAgent()
{
	g_tradeAgent.Logout();
}

SHFU_GATEWAY_EXPORT void __stdcall SetSymbol(const char* symbol)
{
	g_orderProcessor.SetSymbol(std::string(symbol));
}

SHFU_GATEWAY_EXPORT void __stdcall Start(const BreakOutStrategy* pStrategy)
{
	g_orderProcessor.Start(pStrategy);
}

SHFU_GATEWAY_EXPORT void __stdcall Stop()
{
	g_orderProcessor.Stop();
}

SHFU_GATEWAY_EXPORT void __stdcall OpenPosition(int quantity, int longshort)
{
	g_orderProcessor.OpenPosition(quantity, longshort);
}

SHFU_GATEWAY_EXPORT void __stdcall ClosePosition()
{
	g_orderProcessor.ClosePosition();
}

SHFU_GATEWAY_EXPORT void __stdcall ForceClose(const char* symbol, int longshort, int quantity, bool closeYesterday)
{
	g_orderProcessor.ForceClose(std::string(symbol), longshort, quantity, closeYesterday);
}

SHFU_GATEWAY_EXPORT void __stdcall SetQuantity(int quantity)
{
	g_orderProcessor.SetQuantity(quantity);
}

SHFU_GATEWAY_EXPORT void __stdcall EnableStopGain(bool isEnabled, double gainLimit)
{
	g_orderProcessor.EnableStopGain(isEnabled, gainLimit);
}

SHFU_GATEWAY_EXPORT void __stdcall EnableStopLoss(bool isEnabled, double lossLimit)
{
	g_orderProcessor.EnableStopLoss(isEnabled, lossLimit);
}

SHFU_GATEWAY_EXPORT bool __stdcall CancelOrder()
{
	return g_orderProcessor.CancelOrder();
}

SHFU_GATEWAY_EXPORT void __stdcall RegAccountInfo(AccountInfoCallback acctInfoCallback)
{
	g_clientAgent.SetAccountInfoCallback(boost::bind(SendAccountInfo, _1));
	_acctCallbackHandler = acctInfoCallback;
}

SHFU_GATEWAY_EXPORT void __stdcall QueryAccountInfo()
{
	g_tradeAgent.QueryAccount();
}

SHFU_GATEWAY_EXPORT void __stdcall RegPositionDetail(PositionDetailCallback posiDetialCallback)
{
	g_clientAgent.SetPositionDetailCallback(boost::bind(SendPositionDetail, _1));
	_posiDetialCallbackHandler = posiDetialCallback;
}

SHFU_GATEWAY_EXPORT void __stdcall QueryPositionDetail(const char* symbol)
{
	g_tradeAgent.QueryPositionDetails(std::string(symbol));
}

void Cleanup()
{
	g_marketAgent.Logout();
	g_marketAgent.Disconnect();
}

void SendQuoteUpdate(CQuote* pQuote)
{
	if(_quoteCallbackHandler != NULL)
	{
		static QuoteData qd;
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
		_quoteCallbackHandler(&qd);
	}
}

void SendOperationRecords(COperationRecordData* pRecord)
{
	if(_recordCallbackHandler != NULL)
	{
		_recordCallbackHandler(pRecord->InnerStruct());
	}
}

void SendTimeNSales(CTimeNSalePacket* pTns)
{
	if(_tnsCallbackHandler != NULL)
	{
		_tnsCallbackHandler(pTns->InnerStruct());
	}
}

void SendAccountInfo(CAccountInfoMsg* pAcctInfo)
{
	if(_acctCallbackHandler != NULL)
	{
		_acctCallbackHandler(pAcctInfo->InnerStruct());
	}
}

void SendPositionDetail(CPositionDetailMsg* pPosiDetailMsg)
{
	if(_posiDetialCallbackHandler != NULL)
	{
		_posiDetialCallbackHandler(pPosiDetailMsg->InnerStruct());
	}
}