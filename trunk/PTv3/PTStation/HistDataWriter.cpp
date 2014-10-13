#include "StdAfx.h"
#include "HistDataWriter.h"
#include "TechStrategyDefs.h"

#define FIELD_WIDTH 12

using namespace boost::filesystem;

CHistDataWriter::CHistDataWriter(void)
{
}


CHistDataWriter::~CHistDataWriter(void)
{
    if(IsOpen())
        Close();
}

void CHistDataWriter::Init(const string& symbol, int precision, const boost::gregorian::date& tradingDay)
{
	m_symbol = symbol;
	m_precision = precision;
	string tradingDayString = boost::gregorian::to_iso_string(tradingDay);
	m_Name = boost::str(boost::format("%s-%u-%s") % m_symbol % m_precision % tradingDayString);
}

bool CHistDataWriter::Open()
{
	if (!m_Name.empty())
	{
		string filePath = boost::str(boost::format("%s/%s.txt")
			% HIST_DATA_FOLDER % m_Name);
		m_ofstream.open(filePath, std::ofstream::out | std::ofstream::app);
		return m_ofstream.is_open();
	}
	return false;
}

bool CHistDataWriter::Open(const string& symbol, int precision, const boost::gregorian::date& tradingDay)
{
	Init(symbol, precision, tradingDay);
	return Open();
}

void CHistDataWriter::Close()
{
    m_ofstream.close();
}

void CHistDataWriter::Write(const string& timestamp, double open, double high, double low, double close)
{
    if(IsOpen())
    {
		m_ofstream << timestamp << " , " << fixed << setprecision(1);
		m_ofstream << setw(FIELD_WIDTH) << open << ", ";
        m_ofstream << setw(FIELD_WIDTH) << high << ", ";
        m_ofstream << setw(FIELD_WIDTH) << low << ", ";
        m_ofstream << setw(FIELD_WIDTH) << close << endl;
    }
}
