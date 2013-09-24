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

COHLCRecordSet::COHLCRecordSet(const string& symbol, int precision)
	: m_symbol(symbol)
	, m_precision(precision)
{
	bool isIF = boost::starts_with(symbol, "IF");
	int tradingTime = isIF ? IF_TOTAL_TRADING_SECONDS : NON_IF_TOTAL_TRADING_SECONDS ;
	m_countPerDay = tradingTime / precision;
	if(tradingTime % precision > 0)
		m_countPerDay += 1;
	
	int sizeOfVector = 2 * m_countPerDay;
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

// barIdx is 0 based
void COHLCRecordSet::SetToday( int barIdx, double open, double high, double low, double close )
{
	int settingIdx = m_countPerDay + barIdx;
	if(settingIdx > m_lastIndex)
		m_lastIndex = settingIdx;

	OpenSeries[settingIdx] = open;
	HighSeries[settingIdx] = high;
	LowSeries[settingIdx] = low;
	CloseSeries[settingIdx] = close;

#ifdef _DEBUG
	cout << "Setting Today: last Index - " << m_lastIndex << ", last close - " << close << endl; 
#endif
}

void COHLCRecordSet::SetHistory( int barIdx, double open, double high, double low, double close )
{
	OpenSeries[barIdx] = open;
	HighSeries[barIdx] = high;
	LowSeries[barIdx] = low;
	CloseSeries[barIdx] = close;
#ifdef _DEBUG
	cout << "Setting History: bar Index - " << barIdx << ", close - " << close << endl; 
#endif
}
