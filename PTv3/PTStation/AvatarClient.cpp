#include "StdAfx.h"
#include "AvatarClient.h"
#include "ProtobufPacket.h"
#include "ProtocolIDs.h"

CAvatarClient::CAvatarClient(const string& sessionId)
	: m_sessionId(sessionId)
	, m_tradeLogged(false)
	, m_quoteLogged(false)
	, m_destroyed(false)
{
	// Connect quoteAgent and Quote repository
	m_quoteRepositry.Init(&m_quoteAgent);
	m_quoteAgent.SetCallbackHanlder(&m_quoteRepositry);

	m_portfolioMgr.SetQuoteRepository(&m_quoteRepositry);
	m_orderProcessor.Initialize(this, &m_tradeAgent);
}


CAvatarClient::~CAvatarClient(void)
{
	m_destroyed = true;
}

boost::tuple<bool, string> CAvatarClient::TradeLogin( const string& address, const string& brokerId, const string& investorId, const string& password )
{
	if(m_tradeLogged)
		return boost::make_tuple(false, "Trade already Logged in");
	m_investorId = investorId;
	return m_tradeAgent.Login(address, brokerId, investorId, password);
}

void CAvatarClient::TradeLogout()
{
	m_tradeAgent.Logout();
}

boost::tuple<bool, string> CAvatarClient::QuoteLogin( const string& address, const string& brokerId, const string& investorId, const string& password )
{
	if(m_quoteLogged)
		return boost::make_tuple(false, "Quote already Logged in");

	return m_quoteAgent.Login(address, brokerId, investorId, password);
}

void CAvatarClient::QuoteLogout()
{
	m_quoteAgent.Logout();
}

void CAvatarClient::PublishPortfolioUpdate( const entity::PortfolioUpdateItem& portfolioUpdate )
{
	ProtobufPacket<entity::PortfolioUpdateItem> portfUpdateResp(PortfolioUpdateResponseID);
	portfUpdateResp.getData().CopyFrom(portfolioUpdate);
	
	PushPacket(&portfUpdateResp);
}

void CAvatarClient::PublishMultiLegOrderUpdate( trade::MultiLegOrder* pOrder )
{
	ProtobufPacket<trade::MultiLegOrder> mlOrderUpdateResp(MultilegOrderResponseID);
	mlOrderUpdateResp.getData().CopyFrom(*pOrder);

	PushPacket(&mlOrderUpdateResp);
}

void CAvatarClient::PublishLegOrderUpdate( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	ProtobufPacket<entity::LegOrderUpdateParam> legOrdUpdateResp(LegOrderResponseID);
	legOrdUpdateResp.getData().set_portfid(portfId);
	legOrdUpdateResp.getData().set_multilegorderid(mlOrderId);
	legOrdUpdateResp.getData().set_legorderref(legOrd->orderref());

	trade::Order* pOrd = legOrdUpdateResp.getData().mutable_legorder();
	pOrd->CopyFrom(*legOrd);

	PushPacket(&legOrdUpdateResp);
}

void CAvatarClient::PublishTradeUpdate( trade::Trade* pTrade )
{
	ProtobufPacket<trade::Trade> tradeUpdateResp(TradeResponseID);
	tradeUpdateResp.getData().CopyFrom(*pTrade);

	PushPacket(&tradeUpdateResp);
}

void CAvatarClient::PublishPositionDetail( trade::PositionDetailInfo* pPosiDetailInfo )
{

}

