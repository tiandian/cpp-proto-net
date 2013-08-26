#include "StdAfx.h"
#include "TaIndicatorSet.h"


CTaIndicatorSet::CTaIndicatorSet(unsigned int size)
{
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
