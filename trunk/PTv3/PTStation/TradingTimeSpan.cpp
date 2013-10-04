#include "StdAfx.h"
#include "TradingTimeSpan.h"
#include "TechStrategyDefs.h"
#include "SymbolTimeUtil.h"

#define TIME_HOUR_OFFSET 0

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

int CTradingTimeSpan::GetIndex( const boost::chrono::seconds& timePoint )
{
	if(timePoint < m_End)
		return GetIndexFromTime(m_Start, timePoint, m_precision) + m_offset;
	else
		return EndIndex() - 1;
}

int CTradingTimeSpan::GetIndex( const boost::chrono::seconds& timePoint, string* outTimestamp )
{
	if(timePoint < m_End)
	{
		int idx = GetIndexFromTime(m_Start, timePoint, m_precision);
		*outTimestamp = GetISOTimeString(m_Start + boost::chrono::seconds(idx * m_precision));
		return  idx + m_offset;
	}
	else
		return EndIndex() - 1;
}
