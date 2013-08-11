#include "StdAfx.h"
#include "PriceBarDataSource.h"


CTechDataRepo::CTechDataRepo(void)
{
}


CTechDataRepo::~CTechDataRepo(void)
{
}

void CTechDataRepo::Init( int precision )
{
	m_recordSet = OHLCRecordSetPtr(new COHLCRecordSet(precision));
}

CPriceBarDataProxy* CTechDataRepo::Register( const string& symbol, int precision, TA_INDICATOR indicator )
{
	string seriesKey;
	BuildKey(symbol, precision, indicator, &seriesKey);
	PriceBarDSMapIter iterSeries = m_priceBarDSMap.find(seriesKey);
	if(iterSeries != m_priceBarDSMap.end())
	{

	}
	return NULL;
}

void CTechDataRepo::BuildKey( const string& symbol, int precision, TA_INDICATOR indicator, string* outKey )
{
	*outKey = boost::str(boost::format("%s-%d-%d") % symbol % precision % indicator);
}
