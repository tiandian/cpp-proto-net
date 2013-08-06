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

private:
	int m_precision;
};

typedef boost::shared_ptr<COHLCRecordSet> OHLCRecordSetPtr;

