#pragma once

boost::chrono::seconds ParseTimeString(const char* time);
boost::chrono::seconds ParseTimeString(const string& time);

bool isSymbolIF(const string& symbol);
int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision);
string GetISOTimeString(const boost::chrono::seconds& timepoint);

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
    int GetIndex(const boost::chrono::seconds& timePoint)
    {
		if(timePoint < m_End)
			return GetIndexFromTime(m_Start, timePoint, m_precision) + m_offset;
		else
			return EndIndex() - 1;
    }
    // return is 0 based
    int GetIndex(const boost::chrono::seconds& timePoint, string* outTimestamp)
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

private:
    boost::chrono::seconds m_Start;
    boost::chrono::seconds m_End;
    int m_offset;
    int m_endIndex;
    int m_precision;
    
};

typedef boost::shared_ptr<CTradingTimeSpan> TradingTimeSpanPtr;
