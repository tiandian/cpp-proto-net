#pragma once

#include "entity/quote.pb.h"

#include <boost/thread.hpp>

typedef boost::function<void(int, double, double, double, double)> BarChangeFunc;

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

private:
	string m_symbol;
	int m_precision;
	BarChangeFunc m_onBarChanged;
	BarChangeFunc m_onBarFinalized;

	boost::chrono::seconds if_start_1;
	boost::chrono::seconds if_end_1;
	boost::chrono::seconds if_start_2;
	boost::chrono::seconds if_end_2;

	boost::chrono::seconds non_if_start_1;
	boost::chrono::seconds non_if_end_1;
	boost::chrono::seconds non_if_start_2;
	boost::chrono::seconds non_if_end_2;
	boost::chrono::seconds non_if_start_3;
	boost::chrono::seconds non_if_end_3;
};

