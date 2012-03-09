#pragma once

#include "QuoteListener.h"

class COrderManager : public QuoteListener
{
public:
	COrderManager(void);
	~COrderManager(void);

	void Initialize();

	virtual void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);
};

