#pragma once

#include "protobuf_gen/quote.pb.h"

class CMarketAgentCallback
{
public:
	CMarketAgentCallback(void);
	virtual ~CMarketAgentCallback(void);

	virtual void OnSubscribeCompleted() = 0;

	virtual void OnUnsubscribeCompleted() = 0;

	virtual void OnQuoteReceived(CTP::Quote* pQuote) = 0;
};

