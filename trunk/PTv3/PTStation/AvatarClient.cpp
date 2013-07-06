#include "StdAfx.h"
#include "AvatarClient.h"
#include "ProtobufPacket.h"
#include "ProtocolIDs.h"

CAvatarClient::CAvatarClient(const string& sessionId)
	: m_sessionId(sessionId)
	, m_tradeLogged(false)
	, m_quoteLogged(false)
{
	// Connect quoteAgent and Quote repository
	m_quoteRepositry.Init(&m_quoteAgent);
	m_quoteAgent.SetCallbackHanlder(&m_quoteRepositry);

	m_portfolioMgr.SetQuoteRepository(&m_quoteRepositry);
}


CAvatarClient::~CAvatarClient(void)
{
}

boost::tuple<bool, string> CAvatarClient::TradeLogin( const string& address, const string& brokerId, const string& investorId, const string& password )
{
	if(m_tradeLogged)
		return boost::make_tuple(false, "Trade already Logged in");

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

