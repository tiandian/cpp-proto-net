#pragma once

#include "entity/quote.pb.h"

#include <boost/thread.hpp>

typedef boost::function<void(int, double, double, double, double)> BarChangeFunc;

class CPriceBarGen
{
public:
	CPriceBarGen(void);
	~CPriceBarGen(void);

	void Init(int precision);
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
	int m_precision;
	BarChangeFunc m_onBarChanged;
	BarChangeFunc m_onBarFinalized;
};

