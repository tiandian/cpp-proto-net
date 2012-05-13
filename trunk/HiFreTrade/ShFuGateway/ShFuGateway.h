#if !defined(SHFU_GATEWAY_H)
#define SHFU_GATEWAY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EntityStructs.h"

#ifdef SHFUGATEWAY_EXPORTS
#define SHFU_GATEWAY_EXPORT __declspec(dllexport)
#else
#define SHFU_GATEWAY_EXPORT __declspec(dllimport)
#endif

typedef void (__stdcall *QuoteCallback)(const QuoteData* pQuoteData);
typedef void (__stdcall *OperationRecordCallback)(const OperationRecord* pRecord);
typedef void (__stdcall *TimeNSalesCallback)(const TimeNSalesData* pTnsData);

extern "C"
{
	SHFU_GATEWAY_EXPORT int __stdcall TestCall(int a, int b);

	SHFU_GATEWAY_EXPORT bool __stdcall ConnectMarketAgent(	const char* brokerID, 
															const char* userID, 
															const char* password,
															QuoteCallback callbackHandler);

	SHFU_GATEWAY_EXPORT void __stdcall DisconnectMarketAgent();

	SHFU_GATEWAY_EXPORT bool __stdcall ConnectTradeAgent(	const char* brokerID, 
															const char* userID, 
															const char* password,
															OperationRecordCallback recordCallback,
															TimeNSalesCallback tnsCallback);

	SHFU_GATEWAY_EXPORT void __stdcall DisconnectTradeAgent();

	SHFU_GATEWAY_EXPORT void __stdcall SetSymbol(const char* symbol);

	SHFU_GATEWAY_EXPORT void __stdcall Start(const BreakOutStrategy* pStrategy);

	SHFU_GATEWAY_EXPORT void __stdcall Stop();

	SHFU_GATEWAY_EXPORT void __stdcall OpenPosition(int quantity, int longshort);

	SHFU_GATEWAY_EXPORT void __stdcall ClosePosition();

	SHFU_GATEWAY_EXPORT bool __stdcall CancelOrder();

	SHFU_GATEWAY_EXPORT void __stdcall SetQuantity(int quantity);
	
	SHFU_GATEWAY_EXPORT void __stdcall EnableStopGain(bool isEnabled, double gainLimit);
	
	SHFU_GATEWAY_EXPORT void __stdcall EnableStopLoss(bool isEnabled, double lossLimit);
};

#endif
