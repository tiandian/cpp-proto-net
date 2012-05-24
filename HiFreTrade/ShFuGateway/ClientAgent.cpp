#include "StdAfx.h"
#include "ClientAgent.h"
#include "Quote.h"
#include "OperationRecordData.h"
#include "TimeNSalePacket.h"
#include "AccountInfoMsg.h"
#include "PositionDetailMsg.h"
#include "OrderMsg.h"


CClientAgent::CClientAgent(void):
	 m_quoteCallback(NULL)
	,m_recordCallback(NULL)
	,m_tnsUpdateCallback(NULL)
	,m_acctUpdateCallback(NULL)
	,m_positionDetailUpdateCallback(NULL)
	,m_orderUpdateCallback(NULL)
	,m_bufferRunner(boost::bind(&CClientAgent::DispatchMsgPack, this, _1))
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
	else if(msgType == ACCOUNT_INFO)
	{
		CAccountInfoMsg* acct = dynamic_cast<CAccountInfoMsg*>(package.get());
		if(acct != NULL)
			m_acctUpdateCallback(acct);
	}
	else if(msgType == POSITION_DETAIL)
	{
		CPositionDetailMsg* position = dynamic_cast<CPositionDetailMsg*>(package.get());
		if(position != NULL)
		{
			m_positionDetailUpdateCallback(position);
		}
	}
	else if(msgType == ORDER)
	{
		COrderMsg* orderMsg = dynamic_cast<COrderMsg*>(package.get());
		if(orderMsg != NULL)
		{
			m_orderUpdateCallback(orderMsg);
		}
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

void CClientAgent::SetAccountInfoCallback( AccountInfoUpdateFunc callback )
{
	m_acctUpdateCallback = callback;
}

void CClientAgent::SetPositionDetailCallback( PositionDetailUpdateFunc callback )
{
	m_positionDetailUpdateCallback = callback;
}

void CClientAgent::SetOrderCallback( OrderUpdateFunc callback )
{
	m_orderUpdateCallback = callback;
}
