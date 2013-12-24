#include "StdAfx.h"
#include "QuoteStore.h"
#include "QuoteFetcher.h"
#include "globalmembers.h"
#ifdef _DEBUG
#include "SymbolTimeUtil.h"
#endif // _DEBUG


CQuoteStore::CQuoteStore(const string& symbol)
	: m_symbol(symbol)
	, m_isEnd(false)
{
	memset(&m_cachedQuoteData, 0, sizeof(m_cachedQuoteData));
}

CQuoteStore::~CQuoteStore(void)
{
	LOG_DEBUG(logger, boost::str(boost::format("Destructing Quote Store(%s) with %d fetcher ... ") 
		% m_symbol % m_reclaimedFetcher.size()));

	for(vector<CQuoteFetcher*>::iterator iter = m_reclaimedFetcher.begin();
		iter != m_reclaimedFetcher.end(); ++iter)
	{
		if(*iter != NULL)
			delete *iter;
	}

	LOG_DEBUG(logger, boost::str(boost::format("Destructed Quote Store(%s)") 
		% m_symbol));
}

void CQuoteStore::Set( CThostFtdcDepthMarketDataField* pQuoteData, boost::chrono::steady_clock::time_point& tpTimestamp )
{
	// as struct is value type, this should be correct
	boost::unique_lock<boost::mutex> lock(m_quoteMutex);
	m_quoteTimestamp = tpTimestamp;
	m_cachedQuoteData = *pQuoteData;
	m_cond.notify_all();
}

boost::chrono::steady_clock::time_point CQuoteStore::Get( 
	boost::chrono::steady_clock::time_point timestamp, entity::Quote* outQuote )
{
	static boost::chrono::steady_clock::time_point zeroPoint;
	if(timestamp == zeroPoint 
		&& m_quoteTimestamp > zeroPoint)
	{
		// It's the first time of Fetcher getting quote and 
		// store already has cached quote
		GetQuote(outQuote);
	}
	else
	{
		boost::unique_lock<boost::mutex> lock(m_quoteMutex);
		if(!m_isEnd)
		{
			m_cond.wait(lock);

			if(m_quoteTimestamp > timestamp)
				GetQuote(outQuote);
		}
	}
	
	return m_quoteTimestamp;
}

CQuoteFetcher* CQuoteStore::GetFetcher()
{
	boost::unique_lock<boost::mutex> lock(m_fetcherSetMutex);

	CQuoteFetcher* fetcher = new CQuoteFetcher(this);
	m_fetcherSet.insert(fetcher);
	return fetcher;
}

int CQuoteStore::ReclaimFetcher( CQuoteFetcher* pFetcher )
{
	boost::unique_lock<boost::mutex> lock(m_fetcherSetMutex);
	pFetcher->Detach();
	m_fetcherSet.erase(pFetcher);
	m_reclaimedFetcher.push_back(pFetcher);
	
	return m_fetcherSet.size();
}

void CQuoteStore::GetQuote( entity::Quote* outQuote )
{
	outQuote->set_symbol(m_cachedQuoteData.InstrumentID);
	outQuote->set_trading_day(m_cachedQuoteData.TradingDay);
	outQuote->set_exchange_id(m_cachedQuoteData.ExchangeID);
	outQuote->set_exchange_symbol_id(m_cachedQuoteData.ExchangeInstID);
	outQuote->set_last(m_cachedQuoteData.LastPrice);
	outQuote->set_prev_settlement_price(m_cachedQuoteData.PreSettlementPrice);
	outQuote->set_prev_close(m_cachedQuoteData.PreClosePrice);
	outQuote->set_prev_open_interest(m_cachedQuoteData.PreOpenInterest);
	outQuote->set_open(m_cachedQuoteData.OpenPrice);
	outQuote->set_high(m_cachedQuoteData.HighestPrice);
	outQuote->set_low(m_cachedQuoteData.LowestPrice);
	outQuote->set_volume(m_cachedQuoteData.Volume);
	outQuote->set_turnover(m_cachedQuoteData.Turnover);
	outQuote->set_open_interest(m_cachedQuoteData.OpenInterest);
	outQuote->set_close(m_cachedQuoteData.ClosePrice);
	outQuote->set_settlement_price(m_cachedQuoteData.SettlementPrice);
	outQuote->set_upper_limit_price(m_cachedQuoteData.UpperLimitPrice);
	outQuote->set_lower_limit_price(m_cachedQuoteData.LowerLimitPrice);
	outQuote->set_prev_delta(m_cachedQuoteData.PreDelta);
	outQuote->set_curr_delta(m_cachedQuoteData.CurrDelta);

	outQuote->set_update_time(m_cachedQuoteData.UpdateTime);
#ifdef QUOTE_TIME_EMU

	boost::chrono::seconds tp = ParseTimeString(m_cachedQuoteData.UpdateTime);
	tp -= boost::chrono::hours(12);
	string updateTime = GetISOTimeString(tp);
	outQuote->set_update_time(updateTime);
	//outQuote->set_update_time("09:15:58");
	//outQuote->set_open(2308.6);
	//outQuote->set_high(2309.8);
	//outQuote->set_low(2305.4);
	//outQuote->set_last(2306.6);

#endif // QUOTE_TIME_EMU
	
	outQuote->set_update_millisec(m_cachedQuoteData.UpdateMillisec);

	outQuote->set_bid(m_cachedQuoteData.BidPrice1);
	outQuote->set_bid_size(m_cachedQuoteData.BidVolume1);
	outQuote->set_ask(m_cachedQuoteData.AskPrice1);
	outQuote->set_ask_size(m_cachedQuoteData.AskVolume1);
	/*
	outQuote->set_bid_2(m_cachedQuoteData.BidPrice2);
	outQuote->set_bid_size_2(m_cachedQuoteData.BidVolume2);
	outQuote->set_ask_2(m_cachedQuoteData.AskPrice2);
	outQuote->set_ask_size_2(m_cachedQuoteData.AskVolume2);
	outQuote->set_bid_3(m_cachedQuoteData.BidPrice3);
	outQuote->set_bid_size_3(m_cachedQuoteData.BidVolume3);
	outQuote->set_ask_3(m_cachedQuoteData.AskPrice3);
	outQuote->set_ask_size_3(m_cachedQuoteData.AskVolume3);
	outQuote->set_bid_4(m_cachedQuoteData.BidPrice4);
	outQuote->set_bid_size_4(m_cachedQuoteData.BidVolume4);
	outQuote->set_ask_4(m_cachedQuoteData.AskPrice4);
	outQuote->set_ask_size_4(m_cachedQuoteData.AskVolume4);
	outQuote->set_bid_5(m_cachedQuoteData.BidPrice5);
	outQuote->set_bid_size_5(m_cachedQuoteData.BidVolume5);
	outQuote->set_ask_5(m_cachedQuoteData.AskPrice5);
	outQuote->set_ask_size_5(m_cachedQuoteData.AskVolume5);
	*/
	outQuote->set_average_price(m_cachedQuoteData.AveragePrice);
}

void CQuoteStore::EndIfOnlyOneLeft()
{
	boost::unique_lock<boost::mutex> lock(m_quoteMutex);
	if(m_fetcherSet.size() == 1)
	{
		m_quoteTimestamp = boost::chrono::steady_clock::time_point();
		m_isEnd = true;
		m_cond.notify_all();
		LOG_DEBUG(logger, boost::str(boost::format("QuoteStore(%s) Notify all QuoteFetcher to stop with zero timestamp") % m_symbol));
	}
}
