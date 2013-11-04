#include "StdAfx.h"
#include "TaIndicatorSet.h"
#include "globalmembers.h"
#include "TechStrategyDefs.h"

#include <boost/algorithm/string.hpp>

void CTaIndicatorSet::ResetArray(double arr[], int length, double defaultVal)
{
	for(int i = 0; i < length; ++i)
	{
		arr[i] = defaultVal;
	}
}

CTaIndicatorSet::CTaIndicatorSet(int size)
	: m_size(size)
	, m_lastPosition(0)
{
}

CTaIndicatorSet::~CTaIndicatorSet(void)
{
}

double* CTaIndicatorSet::AddIndicator( const string& seriesName, double defaultVal )
{
	DataSeries dataSeries(new double[m_size]);
	m_dataSeriesMap.insert(make_pair(seriesName, dataSeries));
	ResetArray(dataSeries.get(), m_size, defaultVal);
	return dataSeries.get();
}

double* CTaIndicatorSet::FindDataSeries( const string& indicator )
{
	DataSeriesMapIter iter = m_dataSeriesMap.find(indicator);
	if(iter != m_dataSeriesMap.end())
	{
		return (iter->second).get();
	}

	return NULL;
}


double CTaIndicatorSet::GetRef( const string& indicator, int rpos )
{
	double* dataSeries = FindDataSeries(indicator);
	if(dataSeries != NULL)
	{
		int idx = m_lastPosition - rpos;
		if(idx >= 0)
		{
			return dataSeries[idx];
		}
	}

	logger.Warning(boost::str(boost::format("Cannot get ref of %s at rpos:%d") % indicator % rpos));
	return 0;
}

void CTaIndicatorSet::SetRef( const string& indicator, int rpos, double val )
{
	double* dataSeries = FindDataSeries(indicator);
	if(dataSeries != NULL)
	{
		int idx = m_lastPosition - rpos;
		if(idx >= 0)
		{
			dataSeries[idx] = val;
		}
	}
}

void CTaIndicatorSet::Set( const string& indicator, int index, double val )
{
	double* dataSeries = FindDataSeries(indicator);
	if(dataSeries != NULL)
	{
		if(index >= 0 && index < m_size)
		{
			dataSeries[index] = val;
			m_lastPosition = index;
		}
	}
}
