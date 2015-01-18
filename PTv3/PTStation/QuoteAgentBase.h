#pragma once

#include "QuoteAgentCallback.h"

#include <boost/atomic.hpp>
#include <boost/tuple/tuple.hpp>

class CQuoteAgentBase
{
public:
	CQuoteAgentBase(CQuoteAgentCallback* pCallback)
		: m_pCallback(pCallback)
		, m_bIsConnected(false){}
	virtual ~CQuoteAgentBase(){}

	virtual boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password) = 0;
	virtual void Logout() = 0;

	virtual bool SubscribesQuotes(vector<string>& subscribeArr) = 0;
	virtual bool UnSubscribesQuotes(vector<string>& unSubscribeArr) = 0;

	void SetCallbackHanlder(CQuoteAgentCallback* pCallback){ m_pCallback = pCallback; }

	bool IsConnected(){ return m_bIsConnected; }
	bool IsDisconnected(){ return !m_bIsConnected; }

protected:

	CQuoteAgentCallback* m_pCallback;

	boost::atomic<bool> m_bIsConnected;

	string m_brokerID;
	string m_userID;

};

typedef boost::shared_ptr<CQuoteAgentBase> QuoteAgentBasePtr;

