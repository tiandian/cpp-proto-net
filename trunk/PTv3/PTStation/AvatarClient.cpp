#include "StdAfx.h"
#include "AvatarClient.h"


CAvatarClient::CAvatarClient(const string& sessionId)
	: m_sessionId(sessionId)
	, m_tradeLogged(false)
	, m_quoteLogged(false)
{
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

