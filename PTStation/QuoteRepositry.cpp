#include "StdAfx.h"
#include "QuoteRepositry.h"
#include "globalmembers.h"
#include "QuoteAgent.h"

#include <boost/chrono.hpp>

CQuoteRepositry::CQuoteRepositry(void):
m_pQuoteAgent(NULL)
{
}


CQuoteRepositry::~CQuoteRepositry(void)
{
}

CQuoteFetcher* CQuoteRepositry::CreateFetcher( const string& symbol )
{
	boost::unique_lock<boost::mutex> lock(m_storeMapMutex);

	CQuoteStore* pQuoteStore = NULL;
	// check if corresponding quote store exists
	QuoteStoreMapIter iterStore = m_quoteStoreMap.find(symbol);
	if(iterStore != m_quoteStoreMap.end())
	{
		// quote store already existing
		pQuoteStore = (iterStore->second).get();
	}
	else
	{
		QuoteStorePtr quoteStore(new CQuoteStore(symbol));
		m_quoteStoreMap.insert(make_pair(symbol, quoteStore));

		SubscribeQuote(symbol);
		pQuoteStore = quoteStore.get();
	}

	return pQuoteStore->GetFetcher();
}

void CQuoteRepositry::DestoryFetcher( CQuoteFetcher* pFetcher )
{
	boost::unique_lock<boost::mutex> lock(m_storeMapMutex);

	if(pFetcher == NULL)
		return;

	const string& symbol = pFetcher->Symbol();

	QuoteStoreMapIter iterStore = m_quoteStoreMap.find(symbol);
	if(iterStore != m_quoteStoreMap.end())
	{
		(iterStore->second)->EndIfOnlyOneLeft();
		int remainedFetcher = (iterStore->second)->ReclaimFetcher(pFetcher);
		LOG_DEBUG(logger, boost::str(boost::format("Remaining fetcher of quote store(%s): %d.")
			% symbol % remainedFetcher));
		if(remainedFetcher < 1)
		{
			UnsubscribeQuote(symbol);
			m_quoteStoreMap.erase(iterStore);
		}
	}
}

void CQuoteRepositry::OnQuoteReceived( CThostFtdcDepthMarketDataField* marketData, longlong timestamp )
{
	boost::unique_lock<boost::mutex> lock(m_storeMapMutex);

	boost::chrono::steady_clock::duration dd(timestamp);
	boost::chrono::steady_clock::time_point tp(dd);

	const string& symbol = marketData->InstrumentID;
	QuoteStoreMapIter iterStore = m_quoteStoreMap.find(symbol);
	if(iterStore != m_quoteStoreMap.end())
	{
		(iterStore->second)->Set(marketData, tp);
	}
	else
	{
		logger.Warning(boost::str(boost::format("Receiving unexpected quotes for %s")
			% symbol));
	}
}

void CQuoteRepositry::SubscribeQuote( const string& symbol )
{
	if(IsMarketReady())
	{
		vector<string> symbols;
		symbols.push_back(symbol);
		m_pQuoteAgent->SubscribesQuotes(symbols);
	}
}

void CQuoteRepositry::UnsubscribeQuote( const string& symbol )
{
	if(IsMarketReady())
	{
		vector<string> symbols;
		
		symbols.push_back(symbol);
		m_pQuoteAgent->UnSubscribesQuotes(symbols);
	}
}

void CQuoteRepositry::OnConnected(bool reconnected)
{
	vector<string> symbols;
	for(QuoteStoreMapIter iter = m_quoteStoreMap.begin(); iter != m_quoteStoreMap.end(); ++iter)
	{
		symbols.push_back(iter->first);
	}

	m_pQuoteAgent->SubscribesQuotes(symbols);
}

bool CQuoteRepositry::IsMarketReady()
{
	return m_pQuoteAgent->IsConnected();
}
