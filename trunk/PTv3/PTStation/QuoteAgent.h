#pragma once

#define IN_QUOTE_AGENT

#include "ShmQuoteSubscribe.h"
#include "ShmQuoteFeed.h"
#include "QuoteAgentBase.h"

class CQuoteAgent : public CQuoteAgentBase
{
public:
	CQuoteAgent(CQuoteAgentCallback* pCallback);
	~CQuoteAgent(void);

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void Logout();

	bool SubscribesQuotes( vector<string>& subscribeArr );
	bool UnSubscribesQuotes( vector<string>& unSubscribeArr );

private:

	void OnQuotePush(CThostFtdcDepthMarketDataField* mktDataField, longlong timestamp);
	
	void LaunchChildProc(string cmd);

	void PushFakeQuote();

	int m_retChild;
	boost::thread m_thLaunch;

	boost::shared_ptr<CShmQuoteSubscribeProducer> m_quoteSubscriber;
	boost::shared_ptr<CShmQuoteFeedConsumer> m_quoteFeedee;

#ifdef FAKE_QUOTE
	boost::thread m_thFakeQuote;
	bool m_pushingFakeQuote;
#endif
};

