#pragma once

#include <boost/date_time.hpp>
#include <boost/filesystem/fstream.hpp>

class CHistDataWriter
{
public:
    CHistDataWriter(void);
    ~CHistDataWriter(void);
    bool Open(const string& symbol, int precision, const boost::gregorian::date& tradingDay);
    bool IsOpen(){ return m_ofstream.is_open(); }
    void Close();
    void Write(const string& timestamp, double open, double high, double low, double close);

private:
    string m_symbol;
    unsigned int m_precision;
    boost::filesystem::ofstream m_ofstream;
};
