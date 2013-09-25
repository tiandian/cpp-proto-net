#include "StdAfx.h"
#include "TradingTimeSpan.h"
#include "TechStrategyDefs.h"

#include <cstdio>
#include <boost/algorithm/string.hpp>

#define TIME_HOUR_OFFSET 0

boost::chrono::seconds ParseTimeString(const char* time)
{
    boost::chrono::seconds finalVal;
    unsigned int hour = 0, min = 0, sec = 0;
    int got = sscanf(time, "%u:%u:%u", &hour, &min, &sec);
    if(got == 3)
    {
        finalVal = boost::chrono::hours(hour)
            + boost::chrono::minutes(min)
            + boost::chrono::seconds(sec);
    }
    // else must be something wrong
    return finalVal;
}

boost::chrono::seconds ParseTimeString(const string& time)
{
    return ParseTimeString(time.c_str());
}

bool isSymbolIF(const string& symbol)
{
    return boost::starts_with(symbol, IF_PREFIX);
}

// returned index is 0 based
int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision)
{
    boost::chrono::seconds diff = timePoint - baseTp;
    if(diff > boost::chrono::seconds::zero())
    {
        int idx = diff.count() / precision;
        return idx;
    }

    return 0;
}

string GetISOTimeString(const boost::chrono::seconds& timepoint)
{
    boost::chrono::seconds s = timepoint;
    // separate minutes from seconds
    boost::chrono::minutes m = boost::chrono::duration_cast<boost::chrono::minutes>(s);
    s -= m;
    // separate hours from minutes
    boost::chrono::hours h = boost::chrono::duration_cast<boost::chrono::hours>(m);
    m -= h;
    return boost::str(boost::format("%02d:%02d:%02d") % h.count() % m.count() % s.count());
}

CTradingTimeSpan::CTradingTimeSpan( const char* timeBegin, const char* timeEnd, int precision ) :m_precision(precision), m_offset(0)
{
	m_Start = ParseTimeString(timeBegin);
	m_End = ParseTimeString(timeEnd);

#ifdef _DEBUG
	m_Start += boost::chrono::hours(TIME_HOUR_OFFSET);
	m_End += boost::chrono::hours(TIME_HOUR_OFFSET);
#endif

	m_endIndex = GetIndexFromTime(m_Start, m_End, precision);
}
