#pragma once

#include "ThostTraderApi/ThostFtdcMdApi.h"
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

	void Set(CThostFtdcDepthMarketDataField* pQuoteData, boost::chrono::steady_clock::time_point& tpTimestamp);
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

	CThostFtdcDepthMarketDataField m_cachedQuoteData;
	boost::chrono::steady_clock::time_point m_quoteTimestamp;
	bool m_isEnd;
};

typedef boost::shared_ptr<CQuoteStore> QuoteStorePtr;



