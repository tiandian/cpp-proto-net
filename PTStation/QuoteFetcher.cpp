#include "StdAfx.h"
#include "QuoteFetcher.h"
#include "QuoteStore.h"
#include "globalmembers.h"

CQuoteFetcher::CQuoteFetcher(CQuoteStore* pStore):
m_pQuoteStore(pStore)
, m_isRunning(false)
{
}


CQuoteFetcher::~CQuoteFetcher(void)
{
	LOG_DEBUG(logger, boost::str(boost::format("Destructing QuoteFetcher(%s) ...") % Symbol()));
	m_thread.join();
	LOG_DEBUG(logger, boost::str(boost::format("Destructed QuoteFetcher(%s)") % Symbol()));
}

const string& CQuoteFetcher::Symbol()
{
	return m_pQuoteStore->Symbol();
}

void CQuoteFetcher::Run(QuoteUpdateFunc quoteUpdateFunc)
{
	m_cbFunc = quoteUpdateFunc;
	m_isRunning.store(true, boost::memory_order_relaxed);
	m_thread = boost::thread(boost::bind(&CQuoteFetcher::FetchingProc, this));
}

void CQuoteFetcher::Detach()
{
	LOG_DEBUG(logger, boost::str(boost::format("QuoteFetcher(%s) is detached") % Symbol()));
	m_isRunning.store(false, boost::memory_order_release);
	m_cbFunc.clear();
}

void CQuoteFetcher::FetchingProc()
{
	while(m_isRunning.load(boost::memory_order_relaxed))
	{
		static boost::chrono::steady_clock::time_point zeroTP;
		entity::Quote quote;
		boost::chrono::steady_clock::time_point retTimestamp = 
			m_pQuoteStore->Get(m_timestamp, &quote);
		if(retTimestamp > zeroTP)
		{
			if(m_isRunning.load(boost::memory_order_acquire))
			{
				m_timestamp = retTimestamp;
				if(!m_cbFunc.empty())
				{
					m_cbFunc(m_timestamp, &quote);
				}
			}
			else
			{
				LOG_DEBUG(logger, "Quote fetcher has been stop.");
			}
		}
		else
		{
			LOG_DEBUG(logger, "Quote store passing ZERO timestamp for exit");
			break;
		}
	}
}

