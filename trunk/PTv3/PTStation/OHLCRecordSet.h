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
	COHLCRecordSet(const string& symbol, int precision, HISTORY_DATA_MODE histDataMode = ONE_BAR, bool weightAvg = true );
	~COHLCRecordSet(void);

	void SetToday(int barIdx, double open, double high, double low, double close);
	void SetHistory(int barIdx, double open, double high, double low, double close);

	boost::shared_array<double> OpenSeries;
	boost::shared_array<double> HighSeries;
	boost::shared_array<double> LowSeries;
	boost::shared_array<double> CloseSeries;
	boost::shared_array<double> WeightAvgSeries;

	int GetSize(){ return m_totalCount; }
	int GetEndIndex(){ return m_endIndex; }
	int GetBeginIndex() { return m_endIndex - m_nbElements + 1; }
	int NbElements(){ return m_nbElements; }
	int HistoryDataSize() { return m_historyDataSize; }

#ifdef TEST_TODAY_HIST
	// only for debug
	void InitEndIndex() 
	{ 
		m_endIndex = m_historyDataSize - 1; 
		m_nbElements = -1;
	}
#endif // TEST_TODAY_HIST

private:
	static void ResetArray(double arr[], int length);

	string m_symbol;
	int m_precision;
	int m_totalCount;
	int m_historyDataSize;
	int m_endIndex;
	int m_nbElements;

	int m_countInBar;
	double m_totalInBar;

	HISTORY_DATA_MODE m_histDataMode;
	bool m_weightAvg;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

