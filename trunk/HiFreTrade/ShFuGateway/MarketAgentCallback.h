#pragma once

#include "Quote.h"

class CMarketAgentCallback
{
public:
	CMarketAgentCallback(void){}
	virtual ~CMarketAgentCallback(void){}

	virtual void OnSubscribeCompleted() = 0;

	virtual void OnUnsubscribeCompleted() = 0;

	virtual void OnQuoteReceived(CQuote* pQuote) = 0;
};