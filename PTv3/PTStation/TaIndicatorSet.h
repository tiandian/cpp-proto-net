#pragma once

#include <boost/unordered_map.hpp>

typedef vector<double> DataSeries;
typedef boost::shared_ptr<DataSeries> DataSeriesPtr;
typedef boost::unordered_map<string, DataSeriesPtr> DataSeriesMap;
typedef DataSeriesMap::iterator DataSeriesMapIter;

class CTaIndicatorSet
{
public:
	CTaIndicatorSet(unsigned int size);
	virtual ~CTaIndicatorSet(void);

	void Init(const vector<string>& seriesNames);

private:
	DataSeriesMap m_dataSeriesMap;
};

typedef boost::shared_ptr<CTaIndicatorSet> TaIndicatorSetPtr;

