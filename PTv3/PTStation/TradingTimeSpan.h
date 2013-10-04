#pragma once


class CTradingTimeSpan
{
public:
    CTradingTimeSpan(const char* timeBegin, const char* timeEnd, int precision);

    const boost::chrono::seconds& Start(){ return m_Start; }
    const boost::chrono::seconds& End(){ return m_End; }

    void SetOffset(int offset){ m_offset = offset; }
    int Offset(){ return m_offset; }

    int StartIndex(){ return m_offset; }
    int EndIndex(){ return m_offset + m_endIndex; }
    
    bool InScope(const boost::chrono::seconds& timePoint)
    {
        return timePoint >= m_Start && timePoint <= m_End;
    }
	// return is 0 based. if incoming time point equals to m_End, regard it as last point before m_End
    int GetIndex(const boost::chrono::seconds& timePoint);
    // return is 0 based
    int GetIndex(const boost::chrono::seconds& timePoint, string* outTimestamp);

private:
    boost::chrono::seconds m_Start;
    boost::chrono::seconds m_End;
    int m_offset;
    int m_endIndex;
    int m_precision;
    
};

typedef boost::shared_ptr<CTradingTimeSpan> TradingTimeSpanPtr;
