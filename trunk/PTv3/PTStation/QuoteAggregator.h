#pragma once
#include "ShmQuoteSubscribe.h"
#include "ShmQuoteFeed.h"
#include "ThostTraderApi/ThostFtdcMdApi.h"

#include <unordered_map>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

typedef boost::function<void(char** symbolArr, int symCount)> OnSubscribeQuoteFunc;
typedef boost::function<void(void)> OnQuotingEndFunc;

class QuoteTimestamp
{
public:
	QuoteTimestamp(const char* updateTime, int milliseconds)
		: UpdateTime(updateTime), MilliSeconds(milliseconds)
	{}

	string UpdateTime;
	int	MilliSeconds;
	boost::chrono::steady_clock::time_point LastTime;
};

typedef boost::shared_ptr<QuoteTimestamp> TimestampPtr;
typedef unordered_map<string, TimestampPtr> QuoteTimestampMap;
typedef QuoteTimestampMap::iterator QuoteTimestampMapIter;

class CQuoteAggregator
{
public:
	CQuoteAggregator(OnSubscribeQuoteFunc subscribeFunc, OnSubscribeQuoteFunc unsubFunc, OnQuotingEndFunc quoteEndFunc);
	~CQuoteAggregator(void);

	bool Initialize(const string& brokerId, const string& userId);
	void OnQuoteReceived(const string& connectIP, CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
	void SubscribeMarketData(char** symbolArr, int symCount);
	void UnsubscribeMarketData(char** symbolArr, int symCount);
	void OnTerminateNotified();

	boost::shared_ptr<CShmQuoteSubscribeConsumer> m_quoteSubscriber;
	boost::shared_ptr<CShmQuoteFeedProducer> m_quoteFeeder;

	OnSubscribeQuoteFunc m_subscribeFunc;
	OnSubscribeQuoteFunc m_unsubscribeFunc;
	OnQuotingEndFunc m_quotingEndFunc;

	QuoteTimestampMap m_lastQuoteTimestamp;
	boost::mutex m_mutex;
};

