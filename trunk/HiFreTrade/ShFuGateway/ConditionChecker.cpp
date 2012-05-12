#include "StdAfx.h"
#include "ConditionChecker.h"
#include "EntityStructs.h"
#include "LogManager.h"

#include <boost/format.hpp>

extern CLogManager	logger;

COpenPosiCondition::COpenPosiCondition(void)
{
	m_range = 0;
	m_readyForBreakout = false;
	m_periodOK =  false;

	m_rectPeriod = 0;
	m_rectRange = 0;
	m_allowUp = false;
	m_allowDown = false;
	m_criterion = 0;
	m_timespan = 0;

	m_breakingUp = false;
	m_breakingDown = false;
}


COpenPosiCondition::~COpenPosiCondition(void)
{
}

bool COpenPosiCondition::Check( double last, const string& quoteTime, int* offsetFlag )
{
	QuotePtr latestQuote(new Quote(last, quoteTime));

	logger.Trace(boost::str(boost::format("%-8.1f, %s") 
		% last % boost::posix_time::to_simple_string((latestQuote->time()).time_of_day()).c_str()));

	bool openPosition = false;
	if(m_readyForBreakout)
	{
		openPosition = BreakoutCheck(latestQuote, offsetFlag);
	}
	else
	{
		InRangeCheck(latestQuote);
	}

	return openPosition;
}

void COpenPosiCondition::InRangeCheck(const QuotePtr& latestQuote)
{
	m_endTime = latestQuote->time();

	if(m_high.get() == NULL || m_high->price() < latestQuote->price())
		UpdateHigh(latestQuote);

	if(m_low.get() == NULL || m_low->price() > latestQuote->price())
		UpdateLow(latestQuote);

	m_quoteQueue.push_back(latestQuote);

	boost::posix_time::time_duration diff = m_endTime - (m_quoteQueue.front())->time();
	while (diff.seconds() > m_rectPeriod)
	{
		m_periodOK = true;

		bool highPopout = m_quoteQueue.front() == m_high ? true : false ;
		bool lowPopout = m_quoteQueue.front() == m_low ? true : false;

		m_quoteQueue.pop_front();

		if(highPopout)
		{
			logger.Debug("A high pop out, need to find second high");
			UpdateHigh(FindHigh());
		}
		if(lowPopout)
		{
			logger.Debug("A low pop out, need to find second low");
			UpdateLow(FindLow());
		}

		if(m_quoteQueue.empty())
			break;

		diff = m_endTime - (m_quoteQueue.front())->time();
	}

	m_beginTime = m_quoteQueue.front()->time();
	logger.Trace(boost::str(boost::format("Box period: %s --- %s") 
		% boost::posix_time::to_simple_string(m_beginTime.time_of_day()).c_str() 
		% boost::posix_time::to_simple_string(m_endTime.time_of_day()).c_str()));
}

bool COpenPosiCondition::BreakoutCheck(const QuotePtr& latestQuote, int* offsetFlag)
{
	if(!m_breakingUp && !m_breakingDown)
	{
		if(m_allowUp && latestQuote->price() > m_high->price())
		{
			m_breakingUp = true;
			m_breakoutJudgeBegin = latestQuote->time();
			// save new high as breakout high temporarily 
			m_breakoutHigh = latestQuote;
			// Enqueue without pop out
			m_quoteQueue.push_back(latestQuote);
			logger.Debug(boost::str(boost::format("UP breakout happens at: %-8.1f,  %s")
				% latestQuote->price()
				% boost::posix_time::to_simple_string((latestQuote->time()).time_of_day()).c_str()));
			return false;
		}
		
		if(m_allowDown && latestQuote->price() < m_low->price())
		{
			m_breakingDown = true;
			m_breakoutJudgeBegin = latestQuote->time();
			// save new low as breakout low temporarily 
			m_breakoutLow = latestQuote;
			// Enqueue without pop out
			m_quoteQueue.push_back(latestQuote);
			logger.Debug(boost::str(boost::format("DOWN breakout happens at: %-8.1f,  %s")
				% latestQuote->price()
				% boost::posix_time::to_simple_string((latestQuote->time()).time_of_day()).c_str()));
			return false;
		}

		InRangeCheck(latestQuote);
		return false;
	}
	else
	{
		// check if within judge timespan
		boost::posix_time::time_duration breakoutDuration = latestQuote->time() - m_breakoutJudgeBegin;
		if(breakoutDuration.seconds() < m_timespan)
		{
			// if within judge timespan
			if(m_breakingUp && latestQuote->price() > m_breakoutHigh->price())
				m_breakoutHigh = latestQuote;
			if(m_breakingDown && latestQuote->price() < m_breakoutLow->price())
				m_breakoutLow = latestQuote;

			logger.Debug(boost::str(boost::format("In breakout judging timespan quote: %-8.1f, %s")
				% latestQuote->price()
				% boost::posix_time::to_simple_string((latestQuote->time()).time_of_day()).c_str()));
			// Enqueue without pop out
			m_quoteQueue.push_back(latestQuote);
			return false;
		}
		else
		{
			// it's time to determine whether or not to breakout
			double breakpoints = 0;
			
			if(m_breakingUp)
			{
				breakpoints = latestQuote->price() - m_high->price();
				logger.Debug(boost::str(boost::format("Breakout Criterion: %d.  Up break points: %-3.1f") 
					% m_criterion
					% breakpoints));
				if(breakpoints >= m_criterion)
				{
					logger.Debug("UP breakout succeeded.");
					*offsetFlag = LONG_OPEN;
					return true;
				}
				else
				{
					// if failed to breakout
					logger.Debug("UP breakout failed.");
					UpdateHigh(m_breakoutHigh);	// update high
				}
			}

			if(m_breakingDown)
			{
				breakpoints = m_low->price() - latestQuote->price();
				logger.Debug(boost::str(boost::format("Breakout Criterion: %d.  Down break points: %-3.1f") 
					% m_criterion
					% breakpoints));
				if(breakpoints >= m_criterion)
				{
					logger.Debug("DOWN breakout succeeded.");
					*offsetFlag = SHORT_OPEN;
					return true;
				}
				else
				{
					// if failed to breakout
					logger.Debug("DOWN breakout failed.");
					UpdateHigh(m_breakoutLow);	// update high
				}
			}
			
			// clean up variables for breakout judgement
			m_readyForBreakout = false;
			ResetBreakoutJudgement();
			InRangeCheck(latestQuote);
			return false;
		}
	}
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

void COpenPosiCondition::UpdateHigh( const QuotePtr& highQuote )
{
	logger.Trace(boost::str(boost::format("high: %-8.1f => %-8.1f ") % m_high->price() % highQuote->price()));
	m_high = highQuote;
	UpdateRange();
}

void COpenPosiCondition::UpdateLow( const QuotePtr& lowQuote )
{
	logger.Trace(boost::str(boost::format("low: %-8.1f => %-8.1f ") % m_low->price() % lowQuote->price()));
	m_low = lowQuote;
	UpdateRange();
}

void COpenPosiCondition::UpdateRange()
{
	if(m_high.get() != NULL && m_low.get() != NULL)
	{
		logger.Trace(boost::str(boost::format("range: %-4.1f => %-4.1f ") % m_range % (m_high->price() - m_low->price())));
		m_range = m_high->price() - m_low->price();
		if(m_periodOK)
		{
			if(abs(m_range - m_rectRange) < COMPARE_PRECISION)
			{
				m_readyForBreakout = true;
				logger.Info(boost::str(boost::format("Ready for breakout. current range: %-4.1f,  target range: %-4.1f") % m_range % m_rectRange));
			}
			else
				m_readyForBreakout = false;
		}
	}
}
