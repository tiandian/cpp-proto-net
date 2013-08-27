#pragma once

#include "PriceBarGen.h"
#include "HistDataWriter.h"
#include "HistDataReader.h"
#include "OHLCRecordSet.h"
#include "TaIndicatorSet.h"
#include "PriceBarDataProxy.h"
#include "TaCalculator.h"
#include "entity/quote.pb.h"

#include <boost/unordered_map.hpp>


enum TA_INDICATOR
{
	TA_OHLC,
	TA_MACD
};

class CPriceBarDataSource
{
public:
	CPriceBarDataSource(const string& id):m_id(id), m_proxyIdxSeed(0){}
	~CPriceBarDataSource(){}

	const string& Id(){ return m_id; }
	void Init(int precision, TA_INDICATOR indicator);
	CPriceBarDataProxy* AddProxy();
	void RemoveProxy(CPriceBarDataProxy* proxy);
	bool IsDisposable(){ return m_proxiesMap.size() == 0; }

	void InQuote(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);

	COHLCRecordSet* RecordSet(){ return m_recordSet.get(); }
	CTaIndicatorSet* GetTaIndicatorSet(boost::chrono::steady_clock::time_point& timestamp);

private:

	void OnBarChanged(int barIdx, double open, double high, double low, double close);
	void OnBarFinalized(int barIdx, double open, double high, double low, double close);

	typedef boost::unordered_map<unsigned int, PriceBarDataProxyPtr> PriceBarDataProxyMap;
	typedef PriceBarDataProxyMap::iterator PriceBarDataProxyMapIter;
	bool IsDataReady(boost::chrono::steady_clock::time_point& timestamp){ return m_dataTimestamp == timestamp; }

	unsigned int m_proxyIdxSeed;
	boost::mutex m_mutProxyMap;

	boost::condition_variable m_condDataReady;
	boost::mutex m_mutDataReady;
	boost::chrono::steady_clock::time_point m_dataTimestamp;

	string m_id;
	PriceBarDataProxyMap m_proxiesMap;
	OHLCRecordSetPtr m_recordSet;
	TaIndicatorSetPtr m_taIndicatorSet;

	CPriceBarGen m_priceBarGen;
	CHistDataWriter m_histDataWriter;
	CHistDataReader m_histDataReader;
	TaCalculatorPtr m_taCalculator;
};

typedef boost::shared_ptr<CPriceBarDataSource> PriceBarDataSourcePtr;


