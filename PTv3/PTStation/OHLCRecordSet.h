#pragma once

#include <boost/shared_array.hpp>

class COHLCRecordSet
{
public:
	COHLCRecordSet(const string& symbol, unsigned int precision);
	~COHLCRecordSet(void);

	void SetToday(unsigned int barIdx, double open, double high, double low, double close);
	void SetHistory(unsigned int barIdx, double open, double high, double low, double close);

	boost::shared_array<double> OpenSeries;
	boost::shared_array<double> HighSeries;
	boost::shared_array<double> LowSeries;
	boost::shared_array<double> CloseSeries;

	unsigned int GetSize(){ return 2 * m_countPerDay; }
	unsigned int GetLastIndex(){ return m_lastIndex; }
private:
	static void ResetArray(double arr[], int length);

	string m_symbol;
	unsigned int m_precision;
	unsigned int m_countPerDay;
	unsigned int m_lastIndex;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

