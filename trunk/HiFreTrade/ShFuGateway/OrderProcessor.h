#pragma once

#include "MarketAgentCallback.h"
#include "BufferRunner.h"

#include <string>

class COrderProcessor : CMarketAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void SetSymbol(const std::string& symb);

	virtual void OnSubscribeCompleted();
	virtual void OnUnsubscribeCompleted();
	virtual void OnQuoteReceived(CQuote* pQuote);

private:
	void ProcessQuote(boost::shared_ptr<CQuote>& pQuote);

	void ForwardQuote(boost::shared_ptr<CQuote>& pQuote);


	std::string	m_currentSymbol;
	CBufferRunner< boost::shared_ptr<CQuote> > m_bufferRunner;

};

