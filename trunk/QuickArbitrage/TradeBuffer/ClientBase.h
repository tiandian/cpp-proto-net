#pragma once

#include "QuoteListener.h"
#include "BufferRunner.h"
#include "protobuf_gen/quote.pb.h"
#include "MsgPack.h"

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
	
	template<typename T>
	void EnqueueMessage(MSG_TYPE type, boost::shared_ptr<T>& pT)
	{
		boost::shared_ptr<MsgPack> pack(new MsgPackT<T>(type, pT));
		m_pbfRunner->Enqueue(pack);
	}

	virtual void ProcessMessage(MSG_TYPE type, void* pData){}

	virtual void ProcessQuote(CTP::Quote* pQuote){}

private:
	void ProcessMsgPack(boost::shared_ptr<MsgPack>& pPack);

	CBufferRunner< boost::shared_ptr<MsgPack> >* m_pbfRunner;
};

