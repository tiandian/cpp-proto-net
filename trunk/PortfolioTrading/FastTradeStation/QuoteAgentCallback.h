#pragma once

#include "ThostTraderApi/ThostFtdcMdApi.h"

class CQuoteAgentCallback
{
public:
	CQuoteAgentCallback(void){}
	virtual ~CQuoteAgentCallback(void){}

	virtual void OnSubscribeCompleted() = 0;

	virtual void OnUnsubscribeCompleted() = 0;

	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData) = 0;

	virtual void OnConnected(bool reconnected) = 0;
};
