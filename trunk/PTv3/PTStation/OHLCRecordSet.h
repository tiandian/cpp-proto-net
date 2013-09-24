#pragma once

#include <boost/shared_array.hpp>

class COHLCRecordSet
{
public:
	COHLCRecordSet(const string& symbol, int precision);
	~COHLCRecordSet(void);

	void SetToday(int barIdx, double open, double high, double low, double close);
	void SetHistory(int barIdx, double open, double high, double low, double close);

	boost::shared_array<double> OpenSeries;
	boost::shared_array<double> HighSeries;
	boost::shared_array<double> LowSeries;
	boost::shared_array<double> CloseSeries;

	int GetSize(){ return 2 * m_countPerDay; }
	int GetLastIndex(){ return m_lastIndex; }
private:
	static void ResetArray(double arr[], int length);

	string m_symbol;
	int m_precision;
	int m_countPerDay;
	int m_lastIndex;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

