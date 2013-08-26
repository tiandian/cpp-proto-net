#pragma once

class COHLCRecordSet
{
public:
	COHLCRecordSet(int m_precision);
	~COHLCRecordSet(void);

	vector<double> OpenSeries;
	vector<double> HighSeries;
	vector<double> LowSeries;
	vector<double> CloseSeries;

	unsigned int GetSize(){ return m_size; }

private:
	int m_precision;
	unsigned int m_size;
	unsigned int m_lastIndex;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

