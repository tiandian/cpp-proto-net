#pragma once

class COHLCRecordSet
{
public:
	COHLCRecordSet(const string& symbol, int precision);
	~COHLCRecordSet(void);

	void Set(int barIdx, double open, double high, double low, double close);

	vector<double> OpenSeries;
	vector<double> HighSeries;
	vector<double> LowSeries;
	vector<double> CloseSeries;

	unsigned int GetSize(){ return m_size; }

private:
	string m_symbol;
	int m_precision;
	unsigned int m_size;
	unsigned int m_lastIndex;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

