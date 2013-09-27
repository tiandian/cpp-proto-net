#pragma once

#include <boost/shared_array.hpp>

enum HISTORY_DATA_MODE
{
	NONE,
	ONE_DAY,
	ONE_BAR
};

class COHLCRecordSet
{
public:
	COHLCRecordSet(const string& symbol, int precision, HISTORY_DATA_MODE histDataMode = ONE_BAR );
	~COHLCRecordSet(void);

	void SetToday(int barIdx, double open, double high, double low, double close);
	void SetHistory(int barIdx, double open, double high, double low, double close);

	boost::shared_array<double> OpenSeries;
	boost::shared_array<double> HighSeries;
	boost::shared_array<double> LowSeries;
	boost::shared_array<double> CloseSeries;

	int GetSize(){ return m_totalCount; }
	int GetEndIndex(){ return m_endIndex; }
	int GetBeginIndex() { return m_endIndex - m_nbElements + 1; }
	int NbElements(){ return m_nbElements; }

private:
	static void ResetArray(double arr[], int length);

	string m_symbol;
	int m_precision;
	int m_totalCount;
	int m_historyDataSize;
	int m_endIndex;
	int m_nbElements;

	HISTORY_DATA_MODE m_histDataMode;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

