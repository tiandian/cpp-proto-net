#include "StdAfx.h"
#include "HistDataWriter.h"
#include "TechStrategyDefs.h"

using namespace boost::filesystem;

CHistDataWriter::CHistDataWriter(void)
{
}


CHistDataWriter::~CHistDataWriter(void)
{
    if(IsOpen())
        Close();
}

bool CHistDataWriter::Open(const string& symbol, unsigned int precision, const boost::gregorian::date& tradingDay)
{
    m_symbol = symbol;
    m_precision = precision;
    string filePath = boost::str(boost::format("%s/%s-%u-%s.txt") 
        % HIST_DATA_FOLDER % m_symbol % m_precision % tradingDayString);
    m_ofstream.open(filePath, std::ofstream::out | std::ofstream::app);
    return m_ofstream.is_open();
}

void CHistDataWriter::Close()
{
    m_ofstream.close();
}

void CHistDataWriter::Write(const string& timestamp, double open, double high, double low, double close)
{
    if(IsOpen())
    {
        m_ofstream << timestamp << " ";
        m_ofstream << open << ", ";
        m_ofstream << high << ", ";
        m_ofstream << low << ", ";
        m_ofstream << close << endl;
    }
}
