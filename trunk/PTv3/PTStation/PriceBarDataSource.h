#pragma once

#include "OHLCRecordSet.h"
#include "PriceBarDataProxy.h"

#include <boost/unordered_map.hpp>

enum TA_INDICATOR
{
	TA_OHLC,
	TA_MACD
};

class CPriceBarDataSource
{
public:
	CPriceBarDataSource(){}
	~CPriceBarDataSource(){}

private:

};

typedef boost::shared_ptr<CPriceBarDataSource> PriceBarDataSourcePtr;

class CTechDataRepo
{
public:
	CTechDataRepo(void);
	~CTechDataRepo(void);

	CPriceBarDataProxy* Register(const string& symbol, int precision, TA_INDICATOR indicator = TA_OHLC);
	void Init(int precision);
	COHLCRecordSet* RecordSet(){ return m_recordSet.get(); }

private:
	static void BuildKey(const string& symbol, int precision, TA_INDICATOR indicator, string* outKey);
	
	OHLCRecordSetPtr m_recordSet;

	typedef boost::unordered_map<string, PriceBarDataProxyPtr> PriceBarDataProxyMap;
	typedef PriceBarDataProxyMap::iterator PriceBarDataProxyMapIter;
	PriceBarDataProxyMap m_dataProxyMap;

	typedef boost::unordered_map<string, PriceBarDataSourcePtr> PriceBarDSMap;
	typedef PriceBarDSMap::iterator PriceBarDSMapIter;
	PriceBarDSMap m_priceBarDSMap;
};

