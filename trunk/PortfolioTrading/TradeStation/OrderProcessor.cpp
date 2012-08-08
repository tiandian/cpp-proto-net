#include "StdAfx.h"
#include "OrderProcessor.h"
#include "PortfolioOrderHelper.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>


COrderProcessor::COrderProcessor(void):
m_pTradeAgent(NULL)
{
}


COrderProcessor::~COrderProcessor(void)
{
}

void COrderProcessor::OpenOrder( MultiLegOrderPtr multilegOrder )
{
	int count = m_pendingMultiLegOrders.size();
	string mOrderId = boost::str(boost::format("%d") % count);
	multilegOrder->set_orderid(mOrderId);

	m_pendingMultiLegOrders.insert(make_pair(mOrderId, multilegOrder));

	std::vector<boost::shared_ptr<trade::InputOrder>> vecInputOrders;
	int ordCount = GetInputOrders(multilegOrder.get(), &vecInputOrders);
	
	BOOST_FOREACH(const boost::shared_ptr<trade::InputOrder>& iOrd, vecInputOrders)
	{
		m_pTradeAgent->SubmitOrder(iOrd.get());
	}
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

void COrderProcessor::OnRspUserLogin( bool succ, std::string& msg, int initOrderRefID )
{

}
