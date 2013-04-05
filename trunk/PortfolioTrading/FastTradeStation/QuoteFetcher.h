#pragma once

#include "entity/quote.pb.h"

#include <string>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>

using namespace std;

typedef boost::function<void(entity::Quote*)> QuoteUpdateFunc;

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
	long m_timestamp;
};

