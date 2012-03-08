#pragma once

#include "QuoteListener.h"

class COrderManager : public QuoteListener
{
public:
	COrderManager(void);
	~COrderManager(void);

	virtual void OnQuoteRecevied(CTP::Quote* pQuote);
};

