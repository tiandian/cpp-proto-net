#pragma once

#define IN_QUOTE_AGENT

#include "ShmQuoteSubscribe.h"
#include "ShmQuoteFeed.h"
#include "QuoteAgentCallback.h"
#include "ThostTraderApi/ThostFtdcUserApiDataType.h"

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/atomic.hpp>

using namespace std;

class CQuoteAgent
{
public:
	CQuoteAgent(void);
	~CQuoteAgent(void);

	boost::tuple<bool, string> Open( const string& address, const string& streamDir );
	void Close();

	boost::tuple<bool, string> Login(const string& brokerId, const string& userId, const string& password);
	void Logout();

	bool SubscribesQuotes( vector<string>& subscribeArr );
	bool UnSubscribesQuotes( vector<string>& unSubscribeArr );

	void SetCallbackHanlder(CQuoteAgentCallback* pCallback){ m_pCallback = pCallback; }

	bool IsConnected(){ return m_bIsConnected; }
	bool IsDisconnected(){ return !m_bIsConnected; }

private:
	void OnQuotePush(CThostFtdcDepthMarketDataField* mktDataField);
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

