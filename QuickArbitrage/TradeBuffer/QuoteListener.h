#pragma once
class QuoteListener
{
protected:
	QuoteListener(void);
	~QuoteListener(void);

	virtual void OnQuoteRecevied() = 0;
};

