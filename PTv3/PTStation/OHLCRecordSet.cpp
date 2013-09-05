#include "StdAfx.h"
#include "OHLCRecordSet.h"
#include "TechStrategyDefs.h"
#include <boost/algorithm/string.hpp>


COHLCRecordSet::COHLCRecordSet(const string& symbol, unsigned int precision)
	: m_symbol(symbol)
	, m_precision(precision)
{
	bool isIF = boost::starts_with(symbol, "IF");
	unsigned int tradingTime = isIF ? IF_TOTAL_TRADING_SECONDS : NON_IF_TOTAL_TRADING_SECONDS ;
	m_countPerDay = tradingTime / precision;
	if(tradingTime % precision > 0)
		m_countPerDay += 1;
	
	unsigned int sizeOfVector = 2 * m_countPerDay;
	OpenSeries.reserve(sizeOfVector);
	HighSeries.reserve(sizeOfVector);
	LowSeries.reserve(sizeOfVector);
	CloseSeries.reserve(sizeOfVector);
}


COHLCRecordSet::~COHLCRecordSet(void)
{
}

void COHLCRecordSet::SetToday( unsigned int barIdx, double open, double high, double low, double close )
{
	OpenSeries[m_countPerDay + barIdx - 1] = open;
	HighSeries[m_countPerDay + barIdx - 1] = high;
	LowSeries[m_countPerDay + barIdx - 1] = low;
	CloseSeries[m_countPerDay + barIdx - 1] = close;
}

void COHLCRecordSet::SetHistory( unsigned int barIdx, double open, double high, double low, double close )
{
	OpenSeries[barIdx - 1] = open;
	HighSeries[barIdx - 1] = high;
	LowSeries[barIdx - 1] = low;
	CloseSeries[barIdx - 1] = close;
}
