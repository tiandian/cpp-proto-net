#include "StdAfx.h"
#include "OrderProcessor.h"


COrderProcessor::COrderProcessor(void):
m_pTradeAgent(NULL)
{
}


COrderProcessor::~COrderProcessor(void)
{
}

void COrderProcessor::OpenOrder( MultiLegOrderPtr multilegOrder )
{

}

void COrderProcessor::CloseOrder( const string& orderId )
{

}

void COrderProcessor::CancelOrder( const string& orderId )
{

}

void COrderProcessor::Initialize( CTradeAgent* pTradeAgent )
{
	m_pTradeAgent = pTradeAgent;
	if(m_pTradeAgent != NULL)
		m_pTradeAgent->SetCallbackHanlder(this);
}
