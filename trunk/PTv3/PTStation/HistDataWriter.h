#pragma once

#include <boost/date_time.hpp>
#include <boost/filesystem/fstream.hpp>

class CHistDataWriter
{
public:
    CHistDataWriter(void);
    ~CHistDataWriter(void);
	void Init(const string& symbol, int precision, const boost::gregorian::date& tradingDay);
	bool Open();
    bool Open(const string& symbol, int precision, const boost::gregorian::date& tradingDay);
    bool IsOpen(){ return m_ofstream.is_open(); }
    void Close();
    void Write(const string& timestamp, double open, double high, double low, double close);
	const string& Name() { return m_Name; }
private:
    string m_symbol;
	string m_Name;
    unsigned int m_precision;
    boost::filesystem::ofstream m_ofstream;
};
