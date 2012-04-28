#include "StdAfx.h"
#include "ClientAgent.h"


CClientAgent::CClientAgent(void):
	m_bufferRunner(boost::bind(&CClientAgent::DispatchMsgPack, this, _1))
{
}


CClientAgent::~CClientAgent(void)
{
}

void CClientAgent::DispatchMsgPack( boost::shared_ptr<CQuote>& package )
{
	m_quoteCallback(package.get());
}

void CClientAgent::UpdateQuote( boost::shared_ptr<CQuote>& quote )
{
	m_bufferRunner.Enqueue(quote);
}
