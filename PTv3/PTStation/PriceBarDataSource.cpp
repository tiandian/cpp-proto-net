#include "StdAfx.h"
#include "PriceBarDataSource.h"
#include "HistDataReader.h"
#include "globalmembers.h"

#include <boost/date_time.hpp>

void CPriceBarDataSource::Init( const string& symbol, unsigned int precision)
{
	m_symbol = symbol;
	m_precision = precision;
	m_recordSet = OHLCRecordSetPtr(new COHLCRecordSet(symbol, precision));

	CHistDataReader dataReader(symbol, precision);
	dataReader.Read(m_recordSet.get());

	m_priceBarGen.Init(symbol, precision);
	m_priceBarGen.SetBarChangedHandler(boost::bind(&CPriceBarDataSource::OnBarChanged, this, _1, _2, _3, _4, _5));
	m_priceBarGen.SetBarFinalizedHandler(boost::bind(&CPriceBarDataSource::OnBarFinalized, this, _1, _2, _3, _4, _5));
}

CPriceBarDataProxy* CPriceBarDataSource::AddProxy()
{
	boost::mutex::scoped_lock l(m_mutProxyMap);

	unsigned int newIdx = ++m_proxyIdxSeed;
	PriceBarDataProxyPtr proxy(new CPriceBarDataProxy(newIdx, m_symbol, m_precision, this));
	m_proxiesMap.insert(make_pair(newIdx, proxy));
	return proxy.get();
}

void CPriceBarDataSource::RemoveProxy( CPriceBarDataProxy* proxy )
{
	boost::mutex::scoped_lock l(m_mutProxyMap);

	unsigned int proxyIdx = proxy->Index();
	m_proxiesMap.erase(proxyIdx);

}

void CPriceBarDataSource::InQuote( entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp )
{
	boost::unique_lock<boost::mutex> l(m_mutDataReady);
	// ensure to process latest quote, ONLY the first incoming quote with timestamp trigger TA calculation
	if(timestamp > m_dataTimestamp)
	{
		m_dataTimestamp = timestamp;

		m_priceBarGen.Calculate(pQuote);

		m_condDataReady.notify_all();
	}
	else
	{
		LOG_DEBUG(logger, boost::str(boost::format("Ignored incoming quote for %d") % timestamp.time_since_epoch()));
	}
}

COHLCRecordSet* CPriceBarDataSource::GetRecordSet(boost::chrono::steady_clock::time_point& timestamp)
{
	boost::unique_lock<boost::mutex> l(m_mutDataReady);
	bool isLatestReady = m_condDataReady.timed_wait(l, boost::posix_time::seconds(1), 
		boost::bind(&CPriceBarDataSource::IsDataReady, this, timestamp));
	if(isLatestReady)
	{
		return m_recordSet.get();
	}

	return NULL;
}

void CPriceBarDataSource::OnBarChanged( int barIdx, double open, double high, double low, double close )
{
	m_recordSet->Set(barIdx, open, high, low	, close);
}

void CPriceBarDataSource::OnBarFinalized( int barIdx, double open, double high, double low, double close )
{
	m_histDataWriter.Write(open, high, low, close);
}


