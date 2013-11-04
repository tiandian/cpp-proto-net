#pragma once

#include <boost/unordered_map.hpp>
#include <boost/shared_array.hpp>

typedef boost::shared_array<double> DataSeries;
typedef boost::unordered_map<string, DataSeries> DataSeriesMap;
typedef DataSeriesMap::iterator DataSeriesMapIter;

class COHLCRecordSet;

class CTaIndicatorSet
{
public:
	CTaIndicatorSet(int size);
	virtual ~CTaIndicatorSet(void);

	double GetRef(const string& indicator, int rpos);
	
	void SetRef(const string& indicator, int rpos, double val);
	void Set(const string& indicator, int index, double val);

	int GetSize() const { return m_size; }

	virtual void Calculate(COHLCRecordSet* ohlcRecordSet) = 0;

protected:

	static void ResetArray(double arr[], int length, double defaultVal = 0.0);

	double* AddIndicator(const string& seriesName, double defaultVal = 0.0);
	double* FindDataSeries(const string& indicator);

	DataSeriesMap m_dataSeriesMap;
	int m_size;
	int m_lastPosition;
};

typedef boost::shared_ptr<CTaIndicatorSet> TaIndicatorSetPtr;

