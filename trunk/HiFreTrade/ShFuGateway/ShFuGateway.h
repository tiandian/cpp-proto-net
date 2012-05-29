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
typedef void (__stdcall *AccountInfoCallback)(const AccountInfoData* pAcctInfo);
typedef void (__stdcall *PositionDetailCallback)(const PositionDetail* pPositionDetail);
typedef void (__stdcall *OrderCallback)(const OrderInfo* pOrderInfo);

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

	SHFU_GATEWAY_EXPORT void __stdcall ForceClose(const char* symbol, int longshort, int quantity, bool closeYesterday);

	SHFU_GATEWAY_EXPORT bool __stdcall CancelOrder();

	SHFU_GATEWAY_EXPORT void __stdcall SetQuantity(int quantity);
	
	SHFU_GATEWAY_EXPORT void __stdcall EnableStopGain(bool isEnabled, double gainLimit);
	
	SHFU_GATEWAY_EXPORT void __stdcall EnableStopLoss(bool isEnabled, double lossLimit);

	SHFU_GATEWAY_EXPORT void __stdcall RegAccountInfo(AccountInfoCallback acctInfoCallback);

	SHFU_GATEWAY_EXPORT void __stdcall QueryAccountInfo();

	SHFU_GATEWAY_EXPORT void __stdcall RegPositionDetail(PositionDetailCallback posiDetialCallback);

	SHFU_GATEWAY_EXPORT void __stdcall QueryPositionDetail(const char* symbol);

	SHFU_GATEWAY_EXPORT void __stdcall RegOrders(OrderCallback orderCallback);

	SHFU_GATEWAY_EXPORT void __stdcall QueryOrders(const char* symbol);

	SHFU_GATEWAY_EXPORT void __stdcall CancelOrder2(const char* ordRef, 
													const char* exchId, 
													const char* ordSysId, 
													const char* userId, 
													const char* symbol);
};

#endif
