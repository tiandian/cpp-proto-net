#include "StdAfx.h"
#include "ClientAgent.h"
#include "Quote.h"
#include "OperationRecordData.h"
#include "TimeNSalePacket.h"


CClientAgent::CClientAgent(void):
	m_bufferRunner(boost::bind(&CClientAgent::DispatchMsgPack, this, _1))
{
	m_bufferRunner.Start();
}

CClientAgent::~CClientAgent(void)
{
	m_bufferRunner.Stop();
}

void CClientAgent::DispatchMsgPack( boost::shared_ptr<CMessage>& package )
{
	MsgType msgType = package->GetType();
	if(msgType == QUOTE)
	{
		CQuote* quote = dynamic_cast<CQuote*>(package.get());
		if(quote != NULL)
			m_quoteCallback(quote);
	}
	else if(msgType == OPERATION_RECORD)
	{
		COperationRecordData* record = dynamic_cast<COperationRecordData*>(package.get());
		if(record != NULL)
			m_recordCallback(record);
	}
	else if(msgType == TIME_AND_SALES)
	{
		CTimeNSalePacket* tns = dynamic_cast<CTimeNSalePacket*>(package.get());
		if(tns != NULL)
			m_tnsUpdateCallback(tns);
	}
}

void CClientAgent::Publish( boost::shared_ptr<CMessage>& msgPack )
{
	m_bufferRunner.Enqueue(msgPack);
}

void CClientAgent::SetQuoteCallback( QuoteUpdateFunc callback )
{
	m_quoteCallback = callback;
}

void CClientAgent::SetOperationRecordCallback( OperationRecordUpdateFunc callback )
{
	m_recordCallback = callback;
}

void CClientAgent::SetTimeNSalesCallback( TimeNSalesUpdateFunc callback )
{
	m_tnsUpdateCallback = callback;
}
