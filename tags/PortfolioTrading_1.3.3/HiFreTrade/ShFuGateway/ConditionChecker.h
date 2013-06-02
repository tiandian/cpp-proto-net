#pragma once

#include <string>
#include <deque>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;

#define COMPARE_PRECISION 0.01

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

	void setRectPeriod(int period) { m_rectPeriod = period; }
	void setRectRange(double range) { m_rectRange = range; }
	void setAllowUp(bool up) { m_allowUp = up; }
	void setAllowDown(bool down) { m_allowDown = down; }
	void setBreakoutCriterion(double criterion) { m_criterion = criterion; }
	void setBreakoutTimespan(int timespan) { m_timespan = timespan; }

	string RectPeriodBegin() 
	{ 
		return boost::posix_time::to_simple_string(m_beginTime.time_of_day()); 
	}
	string RectPeriodEnd() 
	{	
		return boost::posix_time::to_simple_string(m_endTime.time_of_day()); 
	}
	double High() { return (m_high.get() != NULL ? m_high->price() : 0); }
	double Low() { return (m_low.get() != NULL ? m_low->price() : 0); }
	double Range() { return m_range; }

	void Enable(bool enabled) {}
	void Reset()
	{
		m_range = 0;
		m_readyForBreakout = false;
		m_periodOK =  false;

		m_high.reset();
		m_breakoutHigh.reset();
		m_low.reset();
		m_breakoutLow.reset();

		m_quoteQueue.clear();

		ResetBreakoutJudgement();	
	}

private:

	void UpdateHigh(const QuotePtr& highQuote);

	void UpdateLow(const QuotePtr& lowQuote);

	void UpdateRange();

	const QuotePtr& FindHigh();
	const QuotePtr& FindLow();

	void InRangeCheck(const QuotePtr& latestQuote);
	bool BreakoutCheck(const QuotePtr& latestQuote, int* offsetFlag);

	void ResetBreakoutJudgement()
	{
		m_breakoutJudgeBegin = boost::posix_time::not_a_date_time;
		m_breakingUp = false;
		m_breakingDown = false;
		m_breakoutHigh.reset();
		m_breakoutLow.reset();
	}
	
	std::deque<QuotePtr> m_quoteQueue;
	
	boost::posix_time::ptime m_beginTime;
	boost::posix_time::ptime m_endTime;

	QuotePtr m_high;
	QuotePtr m_breakoutHigh;
	QuotePtr m_low;
	QuotePtr m_breakoutLow;

	double m_range;
	bool m_readyForBreakout;
	bool m_periodOK;

	int m_rectPeriod;
	double m_rectRange;
	bool m_allowUp;
	bool m_allowDown;
	double m_criterion;
	int m_timespan;

	boost::posix_time::ptime m_breakoutJudgeBegin;
	bool m_breakingUp;
	bool m_breakingDown;

	boost::mutex m_mutex;
};

class CStopGainCondition : public CConditionChecker
{
public:
	CStopGainCondition(void):m_gainLimit(0),m_cost(0),m_entryType(0),m_isEnabled(true)
	{}
	~CStopGainCondition(void){}

	bool Check(double last, const string& quoteTime, int* offsetFlag);

	void setGainLimit(double gain) { m_gainLimit = gain; }
	void setCost(double cost) { m_cost = cost; }
	void setEntryType(int entryType) { m_entryType = entryType; }
	void Enable(bool enabled) { m_isEnabled = enabled; }
	bool IsEnabled() { return m_isEnabled; }
	void Reset()
	{
		m_cost = 0;
		m_entryType = 0;
	}

private:
	double m_gainLimit;
	double m_cost;
	int m_entryType;
	bool m_isEnabled;
};

class CStopLossCondition : public CConditionChecker
{
public:
	CStopLossCondition(void):m_lossLimit(0),m_cost(0),m_entryType(0),m_isEnabled(true),m_turningPoint(0)
	{}
	~CStopLossCondition(void){}

	bool Check(double last, const string& quoteTime, int* offsetFlag);

	void setLossLimit(double loss) { m_lossLimit = loss; }
	void setCost(double cost) { m_cost = cost; m_turningPoint = m_cost; }
	void setEntryType(int entryType) { m_entryType = entryType; }
	void Enable(bool enabled) { m_isEnabled = enabled; }
	bool IsEnabled() { return m_isEnabled; }
	void Reset()
	{
		setCost(0);
		m_entryType = 0;
	}
private:
	double m_lossLimit;
	double m_cost;
	int m_entryType;
	bool m_isEnabled;
	double m_turningPoint;
};