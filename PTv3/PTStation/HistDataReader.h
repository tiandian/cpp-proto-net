#pragma once

class COHLCRecordSet;
class CPriceBarGen;

#define HIST_LINE_MAX_LENGTH 256

class CHistDataReader
{
public:
	CHistDataReader(const string& symbol, unsigned int precision);
	~CHistDataReader(void);

	void Read(COHLCRecordSet* pRecordSet, CPriceBarGen* pPriceBarGen);

private:
	bool ParseLine(string* timestamp, double* open, double* high, double* low, double* close);

	string m_symbol;
	unsigned int m_precision;
	char m_buf[HIST_LINE_MAX_LENGTH];

};

