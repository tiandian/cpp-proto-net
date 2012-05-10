#pragma once

#include <string>
#include <deque>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;

class CConditionChecker
{
public:
	CConditionChecker(){}
	virtual ~CConditionChecker(){}

	virtual bool Check(double last, const string& quoteTime, int* offsetFlag) = 0;
};

class COpenPosiCondition : public CConditionChecker
{
	class Quote
	{
	public:
		Quote(double price, const string& timestamp):
		  m_price(price),
			  m_time(	boost::gregorian::day_clock::local_day(), 
			  boost::posix_time::duration_from_string(timestamp))
		  {}
		  ~Quote(){}

		  double price(){return m_price;}
		  boost::posix_time::ptime& time(){return m_time;}

	private:
		double m_price;
		boost::posix_time::ptime m_time;
	};

	typedef boost::shared_ptr<Quote> QuotePtr;

public:
	COpenPosiCondition(void);
	~COpenPosiCondition(void);

	bool Check(double last, const string& quoteTime, int* offsetFlag);

private:

	void UpdateHigh(const QuotePtr& highQuote)
	{
		m_high = highQuote;
		UpdateRange();
	}

	void UpdateLow(const QuotePtr& lowQuote)
	{
		m_low = lowQuote;
		UpdateRange();
	}

	void UpdateRange()
	{
		if(m_high.get() != NULL && m_low.get() != NULL)
		{
			m_range = m_high->price() - m_low->price();
			if(abs(m_range - 5) < 0.2)
				m_readyForBreakout = true;
			else
				m_readyForBreakout = false;
		}
	}

	const QuotePtr& FindHigh();
	const QuotePtr& FindLow();

	void InRangeCheck(double last, const string& quoteTime);
	bool BreakoutCheck(double last, const string& quoteTime, int* offsetFlag);
	
	std::deque<QuotePtr> m_quoteQueue;
	
	boost::posix_time::ptime m_beginTime;
	boost::posix_time::ptime m_endTime;

	QuotePtr m_high;
	QuotePtr m_breakoutHigh;
	QuotePtr m_breakoutLast;
	QuotePtr m_low;
	QuotePtr m_breakoutLow;

	double m_range;
	bool m_readyForBreakout;
};

class CStopGainCondition : public CConditionChecker
{
public:
	CStopGainCondition(void){}
	~CStopGainCondition(void){}

	bool Check(double last, const string& quoteTime, int* offsetFlag){ return false; }
};

class CStopLossCondition : public CConditionChecker
{
public:
	CStopLossCondition(void){}
	~CStopLossCondition(void){}

	bool Check(double last, const string& quoteTime, int* offsetFlag){ return false; }
};