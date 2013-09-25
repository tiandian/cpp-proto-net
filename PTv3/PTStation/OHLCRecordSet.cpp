#include "StdAfx.h"
#include "OHLCRecordSet.h"
#include "TechStrategyDefs.h"
#include "globalmembers.h"

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
	
	m_totalCount = 2 * m_countPerDay;
	OpenSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(OpenSeries.get(), m_totalCount);
	HighSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(HighSeries.get(), m_totalCount);
	LowSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(LowSeries.get(), m_totalCount);
	CloseSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(CloseSeries.get(), m_totalCount);

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

	// protect from out of array boundary
	if(settingIdx < m_totalCount)
	{
		OpenSeries[settingIdx] = open;
		HighSeries[settingIdx] = high;
		LowSeries[settingIdx] = low;
		CloseSeries[settingIdx] = close;
	}

	logger.Info(boost::str(boost::format("Setting Today: last Index - %d, last close - %f")
		% m_lastIndex % close));
}

void COHLCRecordSet::SetHistory( int barIdx, double open, double high, double low, double close )
{
	OpenSeries[barIdx] = open;
	HighSeries[barIdx] = high;
	LowSeries[barIdx] = low;
	CloseSeries[barIdx] = close;

	logger.Info(boost::str(boost::format("Setting History: bar Index - %d, close - %f")
		% barIdx % close));
}
