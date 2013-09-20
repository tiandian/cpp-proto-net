#pragma once

#include <boost/unordered_map.hpp>

typedef vector<double> DataSeries;
typedef boost::shared_ptr<DataSeries> DataSeriesPtr;
typedef boost::unordered_map<string, DataSeriesPtr> DataSeriesMap;
typedef DataSeriesMap::iterator DataSeriesMapIter;

class CTaIndicatorSet
{
public:
	CTaIndicatorSet(const string& symbol, unsigned int precision);
	virtual ~CTaIndicatorSet(void);

	void Init(const vector<string>& seriesNames);

	double GetRef(const string& indicator, int rpos);
	
	void SetRef(const string& indicator, int rpos, double val);
	void Set(const string& indicator, int index, double val);

private:
	DataSeries* FindDataSeries(const string& indicator);

	DataSeriesMap m_dataSeriesMap;
};

typedef boost::shared_ptr<CTaIndicatorSet> TaIndicatorSetPtr;

