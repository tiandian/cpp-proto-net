#include "StdAfx.h"
#include "TradingTimeSpan.h"
#include "TechStrategyDefs.h"

#include <boost/algorithm/string.hpp>

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

unsigned int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, unsigned int precision)
{
    boost::chrono::seconds diff = timePoint - baseTp;
    if(diff > boost::chrono::seconds::zero())
    {
        unsigned int idx = diff.count() / precision;
        if(diff.count() % precision > 0)
            ++idx;

        return idx;
    }
    return 0;
}