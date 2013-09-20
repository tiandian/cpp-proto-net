#pragma once

boost::chrono::seconds ParseTimeString(const char* time);
boost::chrono::seconds ParseTimeString(const string& time);

bool isSymbolIF(const string& symbol);
unsigned int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, unsigned int precision);
string GetISOTimeString(const boost::chrono::seconds& timepoint);

class CTradingTimeSpan
{
public:
    CTradingTimeSpan(const char* timeBegin, const char* timeEnd, unsigned int precision);

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
    
    unsigned int GetIndex(const boost::chrono::seconds& timePoint, string* outTimestamp)
    {
        unsigned int idx = GetIndexFromTime(m_Start, timePoint, m_precision);
        *outTimestamp = GetISOTimeString(m_Start + boost::chrono::seconds(idx * m_precision));
        return  idx + m_offset;
    }

private:
    boost::chrono::seconds m_Start;
    boost::chrono::seconds m_End;
    unsigned int m_offset;
    unsigned int m_endIndex;
    unsigned int m_precision;
    
};

typedef boost::shared_ptr<CTradingTimeSpan> TradingTimeSpanPtr;
