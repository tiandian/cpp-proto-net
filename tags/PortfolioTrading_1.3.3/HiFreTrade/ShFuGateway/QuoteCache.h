#pragma once

#include <string>
#include <boost/thread/mutex.hpp>

class CQuoteCache
{
public:
	CQuoteCache(){}
	~CQuoteCache(){}

	const std::string& Symbol(){ return m_symbol; }
	void setSymbol(const std::string& symb) { m_symbol = symb; }

	double Last()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		return m_last;
	}

	double Ask()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		return m_ask;
	}

	double Bid()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		return m_bid;
	}

	void UpdateQuote(double last, double ask, double bid)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_last = last;
		m_ask = ask;
		m_bid = bid;
	}

private:

	std::string m_symbol;
	double m_last;
	double m_ask;
	double m_bid;

	boost::mutex m_mutex;
};