#include "StdAfx.h"
#include "ClientBase.h"
#include "QuoteAggregator.h"

extern CQuoteAggregator g_quoteAggregator;

ClientBase::ClientBase(void):
	m_pbfRunner(NULL)
{
	m_pbfRunner = new CBufferRunner< boost::shared_ptr<MsgPack> >(boost::bind(&ClientBase::ProcessMsgPack, this, _1));
	m_pbfRunner->Start();
}


ClientBase::~ClientBase(void)
{
	if(m_pbfRunner != NULL)
	{
		m_pbfRunner->Stop();
		delete m_pbfRunner;
	}
}

void ClientBase::OnQuoteRecevied( boost::shared_ptr<CTP::Quote>& pQuote )
{
	// place in buffer runner with QuotePack wrapping
	/*boost::shared_ptr<MsgPack> pack(new MsgPackT<CTP::Quote>(QUOTE, pQuote));
	m_pbfRunner->Enqueue(pack);*/

	EnqueueMessage(QUOTE, pQuote);
}

void ClientBase::Subscribe( vector<string>& symbols )
{
	if(GetUuid().is_nil())
	{
		SetSymbols(symbols);
		g_quoteAggregator.SubscribeQuotes(this);
	}
	else
	{
		if(symbols.size() > 0)
			g_quoteAggregator.ChangeQuotes(this, symbols);
		else
			UnSubscribe();
	}
}

void ClientBase::UnSubscribe()
{
	if(!(GetUuid().is_nil()))
		g_quoteAggregator.UnsubscribeQuotes(GetUuid());
}

void ClientBase::ProcessMsgPack( boost::shared_ptr<MsgPack>& pPack )
{
	MSG_TYPE msgType = pPack->GetType();
	if(msgType == QUOTE)
	{
		void* p_msg = pPack->GetMsg();
		CTP::Quote* pQuote = static_cast<CTP::Quote*>(p_msg);
		ProcessQuote(pQuote);
	}
	else
	{
		ProcessMessage(msgType, pPack->GetMsg());
	}
	/*switch (msgType)
	{
	case QUOTE:
	{

	}
	break;
	case RSP_LOGIN:
	{
	void* p_msg = pPack->GetMsg();
	CTP::Quote* pQuote = static_cast<CTP::Quote*>(p_msg);
	}
	break;
	}*/
}

//void ClientBase::ProcessQuote( CTP::Quote* pQuote )
//{
//	//std::string data;
//	//pQuote->SerializeToString(&data);
//	//BeginWrite(QUOTE, data);	
//	
//	// may block some time before the last sent done.
//	BeginSendMessage(QUOTE, pQuote);
//}
