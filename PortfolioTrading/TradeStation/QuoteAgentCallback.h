#pragma once

#include "../Entity/gen/cpp/quote.pb.h"

class CQuoteAgentCallback
{
public:
	CQuoteAgentCallback(void){}
	virtual ~CQuoteAgentCallback(void){}

	virtual void OnSubscribeCompleted() = 0;

	virtual void OnUnsubscribeCompleted() = 0;

	virtual void OnQuoteReceived(entity::Quote* pQuote) = 0;
};
