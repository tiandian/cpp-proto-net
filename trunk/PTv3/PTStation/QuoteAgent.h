#pragma once

#define IN_QUOTE_AGENT

#include "ShmQuoteSubscribe.h"
#include "ShmQuoteFeed.h"
#include "QuoteAgentCallback.h"

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

class CQuoteAgent
{
public:
	CQuoteAgent(void);
	~CQuoteAgent(void);

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& userId, const string& password);
	void Logout();

	bool SubscribesQuotes( vector<string>& subscribeArr );
	bool UnSubscribesQuotes( vector<string>& unSubscribeArr );

	void SetCallbackHanlder(CQuoteAgentCallback* pCallback){ m_pCallback = pCallback; }

	bool IsConnected(){ return m_bIsConnected; }
	bool IsDisconnected(){ return !m_bIsConnected; }

private:
	void OnQuotePush(CThostFtdcDepthMarketDataField* mktDataField, longlong timestamp);
	void LaunchChildProc(string cmd);

	CQuoteAgentCallback* m_pCallback;

	boost::atomic<bool> m_bIsConnected;
	int m_retChild;
	boost::thread m_thLaunch;

	string m_connString;
	string m_brokerID;
	string m_userID;

	boost::shared_ptr<CShmQuoteSubscribeProducer> m_quoteSubscriber;
	boost::shared_ptr<CShmQuoteFeedConsumer> m_quoteFeedee;
};

