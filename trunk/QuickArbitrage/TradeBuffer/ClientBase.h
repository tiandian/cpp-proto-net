#pragma once

#include "QuoteListener.h"
#include "BufferRunner.h"
#include "protobuf_gen/quote.pb.h"

#include <vector>
#include <string>

using namespace std;

class ClientBase : public QuoteListener
{
public:
	ClientBase(void);
	virtual ~ClientBase(void);

	void Subscribe(vector<string>& symbols);
	void UnSubscribe();

	void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

protected:
	virtual void ProcessQuote(boost::shared_ptr<CTP::Quote>& pQuote) = 0;

private:
	void _internalProcessQuote(boost::shared_ptr<CTP::Quote>& pQuote);

	CBufferRunner< boost::shared_ptr<CTP::Quote> >* m_pbfRunner;
};

