#pragma once
class CMarketAgentCallback
{
public:
	CMarketAgentCallback(void);
	virtual ~CMarketAgentCallback(void);

	virtual void OnSubscribeCompleted() = 0;

	virtual void OnUnsubscribeCompleted() = 0;

	virtual void OnQuoteReceived() = 0;
};

