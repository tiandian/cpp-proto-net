#include "StdAfx.h"
#include "OHLCRecordSet.h"
#include "TechStrategyDefs.h"

#include <boost/algorithm/string.hpp>

void COHLCRecordSet::ResetArray(double arr[], int length)
{
	for(int i = 0; i < length; ++i)
	{
		arr[i] = 0;
	}
}

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
	OpenSeries = boost::shared_array<double>(new double[sizeOfVector]);
	ResetArray(OpenSeries.get(), sizeOfVector);
	HighSeries = boost::shared_array<double>(new double[sizeOfVector]);
	ResetArray(HighSeries.get(), sizeOfVector);
	LowSeries = boost::shared_array<double>(new double[sizeOfVector]);
	ResetArray(LowSeries.get(), sizeOfVector);
	CloseSeries = boost::shared_array<double>(new double[sizeOfVector]);
	ResetArray(CloseSeries.get(), sizeOfVector);

	m_lastIndex = m_countPerDay - 1;
}


COHLCRecordSet::~COHLCRecordSet(void)
{
}

void COHLCRecordSet::SetToday( unsigned int barIdx, double open, double high, double low, double close )
{
	unsigned int settingIdx = m_countPerDay + barIdx - 1;
	if(settingIdx > m_lastIndex)
		m_lastIndex = settingIdx;

	OpenSeries[settingIdx] = open;
	HighSeries[settingIdx] = high;
	LowSeries[settingIdx] = low;
	CloseSeries[settingIdx] = close;
}

void COHLCRecordSet::SetHistory( unsigned int barIdx, double open, double high, double low, double close )
{
	OpenSeries[barIdx - 1] = open;
	HighSeries[barIdx - 1] = high;
	LowSeries[barIdx - 1] = low;
	CloseSeries[barIdx - 1] = close;
}
