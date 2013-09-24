#pragma once

#include "entity/quote.pb.h"
#include "TradingTimeSpan.h"
#include <boost/thread.hpp>

typedef boost::function<void(int, double, double, double, double, const string&)> BarChangeFunc;

class CPriceBarGen
{
public:
	CPriceBarGen(void);
	~CPriceBarGen(void);

	void Init(const string& symbol, int precision);
	void Calculate(entity::Quote* pQuote);

	void SetBarChangedHandler(BarChangeFunc onBarChanged)
	{
		m_onBarChanged = onBarChanged;
	}

	void SetBarFinalizedHandler(BarChangeFunc onBarFinalized)
	{
		m_onBarFinalized = onBarFinalized;
	}

	int GetIndex(const string& quoteTime);
	int GetIndex(const string& quoteTime, string* timestamp);
private:
    
	void RaiseBarChangeEvent(int barIdx, const string& timestamp)
	{
		if(!m_onBarChanged.empty())
			m_onBarChanged(barIdx, m_open, m_high, m_low, m_close, timestamp);
	}

	void RaiseBarFinalizedEvent();

    string m_symbol;
    int m_precision;
	int m_barCount;

	int m_currentIdx;
	string m_currentTimestamp;

	double m_open;
	double m_high;
	double m_low;
	double m_close;
	
	BarChangeFunc m_onBarChanged;
	BarChangeFunc m_onBarFinalized;

    typedef vector<TradingTimeSpanPtr> TimeSpanVec;
    typedef TimeSpanVec::iterator TimeSpanVecIter;
    TimeSpanVec m_vecTimeSpan;

};

