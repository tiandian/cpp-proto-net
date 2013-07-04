#pragma once

#include "entity/quote.pb.h"

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>

typedef boost::function<void(boost::chrono::steady_clock::time_point&, entity::Quote*)> QuoteUpdateFunc;

class CQuoteStore;

class CQuoteFetcher
{
public:
	CQuoteFetcher(CQuoteStore* pStore);
	~CQuoteFetcher(void);

	const string& Symbol();
	void Run(QuoteUpdateFunc quoteUpdateFunc);
	void Detach();

private:
	void FetchingProc();

	CQuoteStore* m_pQuoteStore;
	boost::thread m_thread;
	boost::atomic<bool> m_isRunning;
	
	QuoteUpdateFunc m_cbFunc;
	boost::chrono::steady_clock::time_point m_timestamp;
};

