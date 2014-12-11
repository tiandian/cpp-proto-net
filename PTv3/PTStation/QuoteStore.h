#pragma once

#ifndef USE_FEMAS_API
#include "ThostTraderApi/ThostFtdcMdApi.h"
#else
#include "FemasAPI/USTPFtdcMduserApi.h"
#endif
#include "entity/quote.pb.h"

#include <set>

class CQuoteFetcher;

class CQuoteStore
{
public:
	CQuoteStore(const string& symbol);
	~CQuoteStore(void);

	const string& Symbol(){ return m_symbol; }

	CQuoteFetcher* GetFetcher();
	int ReclaimFetcher(CQuoteFetcher* pFetcher);

#ifndef USE_FEMAS_API
	void Set(CThostFtdcDepthMarketDataField* pQuoteData, boost::chrono::steady_clock::time_point& tpTimestamp);
#else
	void Set(CUstpFtdcDepthMarketDataField* pQuoteData, boost::chrono::steady_clock::time_point& tpTimestamp);
#endif
	
	boost::chrono::steady_clock::time_point Get(
		boost::chrono::steady_clock::time_point timestamp, entity::Quote* outQuote);

	void EndIfOnlyOneLeft();

private:
	void GetQuote(entity::Quote* outQuote);

	string m_symbol;

	set<CQuoteFetcher*> m_fetcherSet;
	vector<CQuoteFetcher*> m_reclaimedFetcher;
	boost::mutex m_fetcherSetMutex;

	boost::condition_variable m_cond;
	boost::mutex m_quoteMutex;

#ifndef USE_FEMAS_API
	CThostFtdcDepthMarketDataField m_cachedQuoteData;
#else
	CUstpFtdcDepthMarketDataField m_cachedQuoteData;
#endif
	
	boost::chrono::steady_clock::time_point m_quoteTimestamp;
	bool m_isEnd;
};

typedef boost::shared_ptr<CQuoteStore> QuoteStorePtr;



