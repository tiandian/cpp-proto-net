#include "StdAfx.h"
#include "HistDataReader.h"
#include "TechStrategyDefs.h"
#include "PriceBarGen.h"
#include "OHLCRecordSet.h"
#include "globalmembers.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

CHistDataReader::CHistDataReader(const string& symbol, unsigned int precision, const boost::gregorian::date& tradingDay)
	: m_symbol(symbol)
	, m_precision(precision)
  , m_tradingDay(tradingDay)
{
}


CHistDataReader::~CHistDataReader(void)
{
}

void CHistDataReader::Read(COHLCRecordSet* pRecordSet, CPriceBarGen* pPriceBarGen)
{
        assert(pRecordSet != NULL);
        assert(pPriceBarGen != NULL);

        // 1. Read yesterday history data
        bool foundLastDayData = false;
        int tryTimes = 0;
        string oldFilePath;
        boost::gregorian::date testTradingDay = m_tradingDay - boost::gregorian::days(1);
        while (!foundLastDayData && tryTimes < 15)
        {
            oldFilePath = GetFilePath(testTradingDay);
            path p(oldFilePath);
            foundLastDayData = exists(p);
            testTradingDay -= boost::gregorian::days(1);
            ++tryTimes;
        }

        if(foundLastDayData)
        {
            ReadFromFile(oldFilePath, pRecordSet, pPriceBarGen, true);
        }
       
        // 2. Read today data if exists
        string todayFilePath = GetFilePath(m_tradingDay);
        path today(todayFilePath);
        if(exists(today))
        {
            ReadFromFile(todayFilePath, pRecordSet, pPriceBarGen, false);
        }
}

string CHistDataReader::GetFilePath(const boost::gregorian::date& tradingDay)
{
    string tradingDayString = boost::gregorian::to_iso_string(tradingDay);
    return boost::str(boost::format("%s/%s-%u-%s.txt") 
        % HIST_DATA_FOLDER % m_symbol % m_precision % tradingDayString);
}

void CHistDataReader::ReadFromFile(const string& dataFilePath, COHLCRecordSet* pRecordSet, CPriceBarGen* pPriceBarGen, bool histData)
{
    ifstream f(dataFilePath);
    if(f.good())
    {
        while(!f.eof())
        {
            f.getline(m_buf, HIST_LINE_MAX_LENGTH);

            string timestamp;
            double open, high, low, close;
            bool succ = ParseLine(&timestamp, &open, &high, &low, &close);
            if(succ)
            {
                unsigned int idx = pPriceBarGen->GetIndex(timestamp);
                if(histData)
                    pRecordSet->SetHistory(idx, open, high, low, close);
                else
                    pRecordSet->SetToday(idx, open, high, low, close);
            }
        }
    }
    else
    {
        logger.Error(boost::str(boost::format("Failed to open file %s") % dataFilePath));
    }
}

bool CHistDataReader::ParseLine( string* timestamp, double* open, double* high, double* low, double* close )
{
	char timeBuf[10];
	int got = sscanf(m_buf, "%s %lf, %lf, %lf, %lf", timeBuf, open, high, low, close);
	*timestamp = timeBuf;
	return got == 5;
}
