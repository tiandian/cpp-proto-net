#include "StdAfx.h"
#include "ConditionChecker.h"


COpenPosiCondition::COpenPosiCondition(void):
m_range(0),m_readyForBreakout(false)
{
}


COpenPosiCondition::~COpenPosiCondition(void)
{
}

bool COpenPosiCondition::Check( double last, const string& quoteTime, int* offsetFlag )
{
	if(m_readyForBreakout)
	{
		BreakoutCheck(last, quoteTime, offsetFlag);
	}
	else
	{
		InRangeCheck(last, quoteTime);
	}

	return false;
}

void COpenPosiCondition::InRangeCheck(double last, const string& quoteTime)
{
	QuotePtr latestQuote(new Quote(last, quoteTime));
	m_endTime = latestQuote->time();

	if(m_high.get() == NULL || m_high->price() < latestQuote->price())
		UpdateHigh(latestQuote);

	if(m_low.get() == NULL || m_low->price() > latestQuote->price())
		UpdateLow(latestQuote);

	m_quoteQueue.push_back(latestQuote);

	boost::posix_time::time_duration diff = m_endTime - (m_quoteQueue.front())->time();
	while (diff.seconds() > 180)
	{
		bool highPopout = m_quoteQueue.front() == m_high ? true : false ;
		bool lowPopout = m_quoteQueue.front() == m_low ? true : false;

		m_quoteQueue.pop_front();

		if(highPopout)
			UpdateHigh(FindHigh());
		if(lowPopout)
			UpdateLow(FindLow());

		if(m_quoteQueue.empty())
			break;

		diff = m_endTime - (m_quoteQueue.front())->time();
	}

	m_beginTime = m_quoteQueue.front()->time();
}

bool COpenPosiCondition::BreakoutCheck(double last, const string& quoteTime, int* offsetFlag)
{
	return false;
}

const COpenPosiCondition::QuotePtr& COpenPosiCondition::FindHigh()
{
	std::deque<QuotePtr>::iterator iterHigh = m_quoteQueue.begin();
	for(std::deque<QuotePtr>::iterator iter = iterHigh + 1;
		iter != m_quoteQueue.end(); ++iter)
	{
		if((*iter)->price() > (*iterHigh)->price())
		{
			iterHigh = iter;
		}
	}
	return *iterHigh;
}

const COpenPosiCondition::QuotePtr& COpenPosiCondition::FindLow()
{
	std::deque<QuotePtr>::iterator iterLow = m_quoteQueue.begin();
	for(std::deque<QuotePtr>::iterator iter = iterLow + 1;
		iter != m_quoteQueue.end(); ++iter)
	{
		if((*iter)->price() < (*iterLow)->price())
		{
			iterLow = iter;
		}
	}
	return *iterLow;
}
