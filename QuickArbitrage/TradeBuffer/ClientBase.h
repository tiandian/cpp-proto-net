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

	void OnQuoteRecevied(CTP::Quote* pQuote);

	void _internalProcessQuote(CTP::Quote* pQuote);

protected:
	virtual void ProcessQuote(CTP::Quote* pQuote) = 0;

private:
	CBufferRunner<CTP::Quote*>* m_pbfRunner;
};

