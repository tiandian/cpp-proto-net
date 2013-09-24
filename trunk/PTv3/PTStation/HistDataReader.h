#pragma once

#include <boost/date_time.hpp>

class COHLCRecordSet;
class CPriceBarGen;

#define HIST_LINE_MAX_LENGTH 256

class CHistDataReader
{
public:
	CHistDataReader(const string& symbol, int precision, const boost::gregorian::date& tradingDay);
	~CHistDataReader(void);

	void Read(COHLCRecordSet* pRecordSet, CPriceBarGen* pPriceBarGen);

private:
  string GetFilePath(const boost::gregorian::date& tradingDay);
  void ReadFromFile(const string& dataFilePath, COHLCRecordSet* pRecordSet, CPriceBarGen* pPriceBarGen, bool histData);
        
	bool ParseLine(string* timestamp, double* open, double* high, double* low, double* close);

	string m_symbol;
	int m_precision;
	char m_buf[HIST_LINE_MAX_LENGTH];
  boost::gregorian::date m_tradingDay;
};

