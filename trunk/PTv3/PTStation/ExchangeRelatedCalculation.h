#pragma once


#define IF_TOTAL_TRADING_SECONDS (4 * 60 * 60 + 30 * 60)
#define NON_IF_TOTAL_TRADING_SECONDS (3 * 60 * 60 + 45 * 60)

#define IF_START_1 "09:15:00"
#define IF_END_1 "11:30:00"
#define IF_START_2 "13:00:00"
#define IF_END_2 "15:15:00"

#define NON_IF_START_1 "09:00:00"
#define NON_IF_END_1 "10:15:00"
#define NON_IF_START_2 "10:30:00"
#define NON_IF_END_2 "11:30:00"
#define NON_IF_START_3 "13:30:00"
#define NON_IF_END_3 "15:00:00"

#define IF_PREFIX "IF"

boost::chrono::seconds ParseTimeString(const char* time);
boost::chrono::seconds ParseTimeString(const string& time);

bool isSymbolIF(const string& symbol);
unsigned int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, unsigned int precision);

class CTradingTimeSpan
{
public:
    CTradingTimeSpan(const char* timeBegin, const char* timeEnd, unsigned int precision)
        :m_precision(precision), m_offset(0)
    {
        m_Start = ParseTimeString(timeBegin);
        m_End = ParseTimeString(timeEnd);
        m_endIndex = GetIndexFromTime(m_Start, m_End, precision);
    }

    const boost::chrono::seconds& Start(){ return m_Start; }
    const boost::chrono::seconds& End(){ return m_End; }

    void SetOffset(int offset){ m_offset = offset; }
    int Offset(){ return m_offset; }

    unsigned int StartIndex(){ return m_offset; }
    unsigned int EndIndex(){ return m_offset + m_endIndex; }
    
    bool InScope(const boost::chrono::seconds& timePoint)
    {
        return timePoint > m_Start && timePoint <= m_End;
    }

    unsigned int GetIndex(const boost::chrono::seconds& timePoint)
    {
        return GetIndexFromTime(m_Start, timePoint, m_precision) + m_offset;
    }

private:
    boost::chrono::seconds m_Start;
    boost::chrono::seconds m_End;
    unsigned int m_offset;
    unsigned int m_endIndex;
    unsigned int m_precision;
    
};

typedef boost::shared_ptr<CTradingTimeSpan> TradingTimeSpanPtr;
