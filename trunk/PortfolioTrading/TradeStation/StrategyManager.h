#pragma once
#include "QuoteAggregator.h"

class CStrategyManager
{
public:
	CStrategyManager(void);
	~CStrategyManager(void);

	void SetQuoteAggregator(CQuoteAggregator* quoteAggregator)
	{
		m_quoteAggregator = quoteAggregator;
	}

	CQuoteAggregator* QuoteAggregator(){ return m_quoteAggregator; }

private:
	CQuoteAggregator* m_quoteAggregator;
};

