#pragma once

#include "QuoteAgent.h"
#include "QuoteUdpAgent.h"

class CQuoteAgentFacade
{
public:
	CQuoteAgentFacade();
	~CQuoteAgentFacade();

	void SetCallbackHanlder(CQuoteAgentCallback* pCallback){ m_pCallback = pCallback; }

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void Logout();

	bool SubscribesQuotes(vector<string>& subscribeArr);
	bool UnSubscribesQuotes(vector<string>& unSubscribeArr);

	bool IsConnected();
	bool IsDisconnected();

private:
	CQuoteAgentCallback* m_pCallback;
	QuoteAgentBasePtr m_quoteAgentUnderlying;
};

