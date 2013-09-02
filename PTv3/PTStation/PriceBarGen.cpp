#include "StdAfx.h"
#include "PriceBarGen.h"


CPriceBarGen::CPriceBarGen(void)
{
}


CPriceBarGen::~CPriceBarGen(void)
{
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

void CPriceBarGen::Calculate(const string& quoteTime)
{
    int barIdx = GetIndex(quoteTime);
    cout << quoteTime << ": " << barIdx << endl;
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
