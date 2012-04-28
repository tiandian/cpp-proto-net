#pragma once

#include "Quote.h"
#include "BufferRunner.h"


typedef boost::function<void(CQuote*)> QuoteUpdateFunc;

class CClientAgent
{
public:
	CClientAgent(void);
	~CClientAgent(void);

	void SetQuoteCallback(QuoteUpdateFunc callback)
	{
		m_quoteCallback = callback;
	}

	void UpdateQuote(boost::shared_ptr<CQuote>& quote);

private:
	void DispatchMsgPack(boost::shared_ptr<CQuote>& package);


	QuoteUpdateFunc m_quoteCallback;
	CBufferRunner< boost::shared_ptr<CQuote> > m_bufferRunner;
};

