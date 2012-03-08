#pragma once

#include "QuoteListener.h"

class COrderManager : public QuoteListener
{
public:
	COrderManager(void);
	~COrderManager(void);

	void Initialize();

	virtual void OnQuoteRecevied(CTP::Quote* pQuote);
};

