#include "StdAfx.h"
#include "TaIndicatorSet.h"
#include "globalmembers.h"
#include "TechStrategyDefs.h"

#include <boost/algorithm/string.hpp>

CTaIndicatorSet::CTaIndicatorSet(const string& symbol, unsigned int precision)
{
	bool isIF = boost::starts_with(symbol, "IF");
	unsigned int tradingTime = isIF ? IF_TOTAL_TRADING_SECONDS : NON_IF_TOTAL_TRADING_SECONDS ;
	int countPerDay = tradingTime / precision;
	if(tradingTime % precision > 0)
		countPerDay += 1;

	unsigned int sizeOfVector = 2 * countPerDay;

	for(DataSeriesMapIter iter = m_dataSeriesMap.begin(); iter != m_dataSeriesMap.end(); ++iter)
	{
		(iter->second)->reserve(sizeOfVector);
	}
}


CTaIndicatorSet::~CTaIndicatorSet(void)
{
}

void CTaIndicatorSet::Init( const vector<string>& seriesNames )
{
	for(vector<string>::const_iterator iter = seriesNames.begin();
		iter != seriesNames.end(); ++iter)
	{
		DataSeriesPtr dataSeries(new DataSeries);
		m_dataSeriesMap.insert(make_pair(*iter, dataSeries));
	}
}

double CTaIndicatorSet::GetRef( const string& indicator, int rpos )
{
	DataSeries* dataSeries = FindDataSeries(indicator);
	if(dataSeries != NULL)
	{
		long size = dataSeries->size();
		int idx = size - rpos - 1;
		if(idx >= 0)
		{
			return dataSeries->at(idx);
		}
	}

	logger.Warning(boost::str(boost::format("Cannot get ref of %s at rpos:%d") % indicator % rpos));
	return 0;
}

void CTaIndicatorSet::SetRef( const string& indicator, int rpos, double val )
{
	DataSeries* dataSeries = FindDataSeries(indicator);
	if(dataSeries != NULL)
	{
		long size = dataSeries->size();
		int idx = size - rpos - 1;
		if(idx >= 0)
		{
			(*dataSeries)[idx] = val;
		}
	}
}

void CTaIndicatorSet::Set( const string& indicator, int index, double val )
{
	DataSeries* dataSeries = FindDataSeries(indicator);
	if(dataSeries != NULL)
	{
		if(index >= 0)
		{
			(*dataSeries)[index] = val;
		}
	}
}

DataSeries* CTaIndicatorSet::FindDataSeries( const string& indicator )
{
	DataSeriesMapIter iter = m_dataSeriesMap.find(indicator);
	if(iter != m_dataSeriesMap.end())
	{
		return (iter->second).get();
	}
	return NULL;
}
