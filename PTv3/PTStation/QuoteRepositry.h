#pragma once

#include "QuoteAgentCallback.h"
#include "QuoteFetcher.h"
#include "QuoteStore.h"

class CQuoteAgentFacade;

class CQuoteRepositry : public CQuoteAgentCallback
{
public:
	CQuoteRepositry(void);
	~CQuoteRepositry(void);

	// CQuoteAgentCallback override virtual functions
	virtual void OnSubscribeCompleted(){}
	virtual void OnUnsubscribeCompleted(){}
	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp);
	
	virtual void OnConnected(bool reconnected);

	void Init(CQuoteAgentFacade* pQuoteAgent){ m_pQuoteAgent = pQuoteAgent; }
	CQuoteFetcher* CreateFetcher(const string& symbol);
	void DestoryFetcher(CQuoteFetcher* pFetcher);

private:
	bool IsMarketReady();
	void SubscribeQuote(const string& symbol);
	void UnsubscribeQuote(const string& symbol);

	typedef map<string, QuoteStorePtr> QuoteStoreMap;
	typedef QuoteStoreMap::iterator QuoteStoreMapIter;
	
	QuoteStoreMap m_quoteStoreMap;
	boost::mutex m_storeMapMutex;

	CQuoteAgentFacade* m_pQuoteAgent;
	
};

