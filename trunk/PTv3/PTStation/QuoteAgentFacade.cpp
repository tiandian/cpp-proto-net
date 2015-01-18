#include "stdafx.h"
#include "QuoteAgentFacade.h"

#include <boost/algorithm/string.hpp>

CQuoteAgentFacade::CQuoteAgentFacade()
{
}


CQuoteAgentFacade::~CQuoteAgentFacade()
{
}

bool IfQuoteUdpBroadcast(const string& frontAddr)
{
	return  boost::istarts_with(frontAddr, "udp://255.255.255.255");
}

boost::tuple<bool, string> CQuoteAgentFacade::Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	if (IfQuoteUdpBroadcast(frontAddr))
	{
		m_quoteAgentUnderlying = QuoteAgentBasePtr(new CQuoteUdpAgent(m_pCallback));
	}
	else
	{
		m_quoteAgentUnderlying = QuoteAgentBasePtr(new CQuoteAgent(m_pCallback));
	}

	return m_quoteAgentUnderlying->Login(frontAddr, brokerId, investorId, userId, password);
}

void CQuoteAgentFacade::Logout()
{
	if (m_quoteAgentUnderlying.get() != NULL)
		m_quoteAgentUnderlying->Logout();

	m_quoteAgentUnderlying.reset();
}

bool CQuoteAgentFacade::SubscribesQuotes(vector<string>& subscribeArr)
{
	if (m_quoteAgentUnderlying.get() != NULL)
		return m_quoteAgentUnderlying->SubscribesQuotes(subscribeArr);

	return false;
}

bool CQuoteAgentFacade::UnSubscribesQuotes(vector<string>& unSubscribeArr)
{
	if (m_quoteAgentUnderlying.get() != NULL)
		return m_quoteAgentUnderlying->UnSubscribesQuotes(unSubscribeArr);

	return false;
}

bool CQuoteAgentFacade::IsConnected()
{
	if (m_quoteAgentUnderlying.get() != NULL)
		return m_quoteAgentUnderlying->IsConnected();

	return false;
}

bool CQuoteAgentFacade::IsDisconnected()
{
	if (m_quoteAgentUnderlying.get() != NULL)
		return m_quoteAgentUnderlying->IsDisconnected();

	return true;
}
