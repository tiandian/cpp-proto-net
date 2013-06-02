#include "StdAfx.h"
#include "ClientBase.h"
#include "QuoteAggregator.h"
#include "OrderManager.h"

extern CQuoteAggregator g_quoteAggregator;
extern COrderManager g_orderMgr;

ClientBase::ClientBase(void):
	m_pbfRunner(NULL), m_tradeLoggedin(false)
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
}

bool ClientBase::Login( const std::string& brokerId, const std::string& userId, const std::string& password )
{
	m_brokerId = brokerId;
	m_userId = userId;
	return g_orderMgr.Register(this, m_brokerId, m_userId, const_cast<std::string&>(password));
}

void ClientBase::Logout()
{
	if(m_tradeLoggedin)
	{
		g_orderMgr.Unregister(m_brokerId, m_userId);
		m_tradeLoggedin = false;
	}
}
