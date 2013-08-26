#pragma once

#include "PriceBarDataSource.h"
#include <boost/unordered_map.hpp>

class CPriceBarDataProxy;

class CTechDataRepo
{
public:
	CTechDataRepo(void);
	~CTechDataRepo(void);

	CPriceBarDataProxy* Register(const string& symbol, int precision, TA_INDICATOR indicator = TA_OHLC, map<string, double>* indicatorParams = NULL);
	bool Unregister(CPriceBarDataProxy* proxy);

private:
	static void BuildKey(const string& symbol, int precision, TA_INDICATOR indicator, string* outKey);

	typedef boost::unordered_map<string, PriceBarDataSourcePtr> PriceBarDSMap;
	typedef PriceBarDSMap::iterator PriceBarDSMapIter;
	PriceBarDSMap m_priceBarDSMap;
};