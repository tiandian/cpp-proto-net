#pragma once

class COHLCRecordSet
{
public:
	COHLCRecordSet(const string& symbol, unsigned int precision);
	~COHLCRecordSet(void);

	void SetToday(unsigned int barIdx, double open, double high, double low, double close);
	void SetHistory(unsigned int barIdx, double open, double high, double low, double close);

	vector<double> OpenSeries;
	vector<double> HighSeries;
	vector<double> LowSeries;
	vector<double> CloseSeries;

	unsigned int GetSize(){ return 2 * m_countPerDay; }

private:
	string m_symbol;
	unsigned int m_precision;
	unsigned int m_countPerDay;
	unsigned int m_lastIndex;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

