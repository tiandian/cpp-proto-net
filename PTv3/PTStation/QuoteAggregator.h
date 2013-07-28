#pragma once
#include "ShmQuoteSubscribe.h"
#include "ShmQuoteFeed.h"
#include "ThostTraderApi/ThostFtdcMdApi.h"

#include <boost/bind.hpp>

typedef boost::function<void(char** symbolArr, int symCount)> OnSubscribeQuoteFunc;
typedef boost::function<void(void)> OnQuotingEndFunc;

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
};

