#include "StdAfx.h"
#include "OHLCRecordSet.h"
#include "ExchangeRelatedCalculation.h"
#include <boost/algorithm/string.hpp>


COHLCRecordSet::COHLCRecordSet(const string& symbol, int precision)
	: m_symbol(symbol)
	, m_precision(precision)
{
	bool isIF = boost::starts_with(symbol, "IF");
	unsigned int tradingTime = isIF ? IF_TOTAL_TRADING_SECONDS : NON_IF_TOTAL_TRADING_SECONDS ;
	m_size = tradingTime / precision;
	if(tradingTime % precision > 0)
		m_size += 1;
}


COHLCRecordSet::~COHLCRecordSet(void)
{
}

void COHLCRecordSet::Set( int barIdx, double open, double high, double low, double close )
{
	OpenSeries[barIdx] = open;
	HighSeries[barIdx] = high;
	LowSeries[barIdx] = low;
	CloseSeries[barIdx] = close;
}
