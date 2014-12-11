#pragma once

#ifndef USE_FEMAS_API
#include "ThostTraderApi/ThostFtdcMdApi.h"
#else
#include "FemasAPI/USTPFtdcMduserApi.h"
#endif

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

#ifndef USE_FEMAS_API
	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp) = 0;
#else
	virtual void OnQuoteReceived(CUstpFtdcDepthMarketDataField* marketData, longlong timestamp) = 0;
#endif

	virtual void OnConnected(bool reconnected) = 0;
};
