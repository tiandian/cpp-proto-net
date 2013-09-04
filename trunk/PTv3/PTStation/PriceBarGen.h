#pragma once

#include "entity/quote.pb.h"
#include "TradingTimeSpan.h"
#include <boost/thread.hpp>

typedef boost::function<void(int, double, double, double, double)> BarChangeFunc;

class CPriceBarGen
{
public:
	CPriceBarGen(void);
	~CPriceBarGen(void);

	void Init(const string& symbol, unsigned int precision);
	void Calculate(entity::Quote* pQuote);

	void SetBarChangedHandler(BarChangeFunc onBarChanged)
	{
		m_onBarChanged = onBarChanged;
	}

	void SetBarFinalizedHandler(BarChangeFunc onBarFinalized)
	{
		m_onBarFinalized = onBarFinalized;
	}

	unsigned int GetIndex(const string& quoteTime);
  unsigned int GetIndex(const string& quoteTime, string* timestamp);
private:
    

    string m_symbol;
    int m_precision;
	BarChangeFunc m_onBarChanged;
	BarChangeFunc m_onBarFinalized;

    typedef vector<TradingTimeSpanPtr> TimeSpanVec;
    typedef TimeSpanVec::iterator TimeSpanVecIter;
    TimeSpanVec m_vecTimeSpan;
};

