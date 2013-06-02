#pragma once

#include "ThostTraderApi/ThostFtdcMdApi.h"

#ifdef WIN32
#define longlong __int64
#else
#define longlong long long
#endif

class CQuoteAgentCallback
{
public:
	CQuoteAgentCallback(void){}
	virtual ~CQuoteAgentCallback(void){}

	virtual void OnSubscribeCompleted() = 0;

	virtual void OnUnsubscribeCompleted() = 0;

	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp) = 0;

	virtual void OnConnected(bool reconnected) = 0;
};
