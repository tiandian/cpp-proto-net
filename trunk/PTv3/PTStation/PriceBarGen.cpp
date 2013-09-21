#include "StdAfx.h"
#include "PriceBarGen.h"
#include "TechStrategyDefs.h"


CPriceBarGen::CPriceBarGen(void)
	: m_currentIdx(0)
	, m_open(0)
	, m_high(0)
	, m_low(0)
	, m_close(0)
{
}


CPriceBarGen::~CPriceBarGen(void)
{
	// Write the last quote to file
	RaiseBarFinalizedEvent();
}


void CPriceBarGen::Init(const string& symbol, unsigned int precision)
{
    bool isIF = isSymbolIF(symbol);
    if(isIF)
    {
        TradingTimeSpanPtr if_span_1(new CTradingTimeSpan(IF_START_1, IF_END_1, precision));
        m_vecTimeSpan.push_back(if_span_1);
        TradingTimeSpanPtr if_span_2(new CTradingTimeSpan(IF_START_2, IF_END_2, precision));
        m_vecTimeSpan.push_back(if_span_2);
        if_span_2->SetOffset(if_span_1->EndIndex());
        
    }
    else
    {
        TradingTimeSpanPtr non_if_span_1(new CTradingTimeSpan(NON_IF_START_1, NON_IF_END_1, precision));
        m_vecTimeSpan.push_back(non_if_span_1);
        TradingTimeSpanPtr non_if_span_2(new CTradingTimeSpan(NON_IF_START_2, NON_IF_END_2, precision));
        m_vecTimeSpan.push_back(non_if_span_2);
        non_if_span_2->SetOffset(non_if_span_1->EndIndex());
        TradingTimeSpanPtr non_if_span_3(new CTradingTimeSpan(NON_IF_START_3, NON_IF_END_3, precision));
        m_vecTimeSpan.push_back(non_if_span_3);
        non_if_span_3->SetOffset(non_if_span_2->EndIndex());
    }
}

void CPriceBarGen::Calculate(entity::Quote* pQuote)
{
    string timestamp;
    unsigned int barIdx = GetIndex(pQuote->update_time(), &timestamp);
	double last = pQuote->last();;
	if(barIdx > m_currentIdx)
	{
		// if not the first one, finalize the last bar
		RaiseBarFinalizedEvent();

		m_currentIdx = barIdx;
		m_currentTimestamp = timestamp;
		m_close = m_low = m_high = m_open = last;
	}
	else if(barIdx == m_currentIdx)
	{
		m_close = last;
		if(last > m_high)
			m_high = last;
		if(last < m_low)
			m_low = last;
	}
	else // barIdx < m_currentIdx ???
		assert(barIdx > m_currentIdx);

	RaiseBarChangeEvent(barIdx, timestamp);
}

unsigned int CPriceBarGen::GetIndex(const string& quoteTime, string* timestamp)
{
    boost::chrono::seconds quoteTimePoint = ParseTimeString(quoteTime);

    for(TimeSpanVecIter iter = m_vecTimeSpan.begin();
        iter != m_vecTimeSpan.end(); ++iter)
    {
        if((*iter)->InScope(quoteTimePoint))
        {
            return (*iter)->GetIndex(quoteTimePoint, timestamp);
        }
    }

    // input quote time is NOT in valid trading time
    return 0;
}

unsigned int CPriceBarGen::GetIndex(const string& quoteTime)
{
    boost::chrono::seconds quoteTimePoint = ParseTimeString(quoteTime);

    for(TimeSpanVecIter iter = m_vecTimeSpan.begin();
        iter != m_vecTimeSpan.end(); ++iter)
    {
        if((*iter)->InScope(quoteTimePoint))
        {
            return (*iter)->GetIndex(quoteTimePoint);
        }
    }

    // input quote time is NOT in valid trading time
    return 0;
}

void CPriceBarGen::RaiseBarFinalizedEvent()
{
	if(m_currentIdx > 0 && !m_onBarFinalized.empty())
	{
		m_onBarFinalized(m_currentIdx, m_open, m_high, m_low, m_close, m_currentTimestamp);
	}
}
