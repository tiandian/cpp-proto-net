#pragma once

#include "QuoteAgentBase.h"
#include "UdpQuoteListener.h"

#include <set>

class CQuoteUdpAgent : public CQuoteAgentBase
{
public:
	CQuoteUdpAgent(CQuoteAgentCallback* pCallback);
	~CQuoteUdpAgent();

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void Logout();

	bool SubscribesQuotes(vector<string>& subscribeArr);
	bool UnSubscribesQuotes(vector<string>& unSubscribeArr);

private:
	void OnUdpDataReceived(char* pData, std::size_t nSize);

	UdpQuoteListenerPtr m_udpListener;
	
	set<string> m_symbols;
	boost::mutex m_mutSymbol;
};

