#include "stdafx.h"
#include "QuoteUdpAgent.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

CQuoteUdpAgent::CQuoteUdpAgent(CQuoteAgentCallback* pCallback)
	: CQuoteAgentBase(pCallback)
{
}

CQuoteUdpAgent::~CQuoteUdpAgent()
{
}

int GetQuoteBroadcastPort(const string& frontAddr)
{
	bool isUdp = boost::istarts_with(frontAddr, "udp://255.255.255.255");
	int nPort = -1;
	if (isUdp)
	{
		string portStr = frontAddr.substr(22);
		try
		{
			nPort = boost::lexical_cast<int>(portStr);
		}
		catch (const boost::bad_lexical_cast &)
		{
		}
	}
	return nPort;
}

boost::tuple<bool, string> CQuoteUdpAgent::Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	m_brokerID = brokerId;
	m_userID = investorId;

	int nPort = GetQuoteBroadcastPort(frontAddr);
	if (nPort > 0)
	{
		m_udpListener = UdpQuoteListenerPtr(
			new CUdpQuoteListener(nPort,
			boost::bind(&CQuoteUdpAgent::OnUdpDataReceived, this, _1, _2)));

		m_bIsConnected = true;
		return boost::make_tuple(true, "");
	}
	else
	{
		return boost::make_tuple(false, "Cannot find port from connection string");
	}
}

void CQuoteUdpAgent::Logout()
{
	if (m_bIsConnected)
	{
		m_bIsConnected = false;
		m_udpListener.reset();
	}
}

bool CQuoteUdpAgent::SubscribesQuotes(vector<string>& subscribeArr)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);

	for (vector<string>::iterator iter = subscribeArr.begin(); iter != subscribeArr.end(); ++iter)
	{
		m_symbols.insert(*iter);
	}
	return true;
}

bool CQuoteUdpAgent::UnSubscribesQuotes(vector<string>& unSubscribeArr)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);
	for (vector<string>::iterator iter = unSubscribeArr.begin(); iter != unSubscribeArr.end(); ++iter)
	{
		set<string>::iterator iterFound = m_symbols.find(*iter);
		{
			if (iterFound != m_symbols.end())
			{
				m_symbols.erase(iterFound);
			}
		}
	}
	return true;
}

void CQuoteUdpAgent::OnUdpDataReceived(char* pData, std::size_t nSize)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);

	CThostFtdcDepthMarketDataField *pDepthMarketData = reinterpret_cast<CThostFtdcDepthMarketDataField*>(pData);
	
	set<string>::iterator iterFound = m_symbols.find(pDepthMarketData->InstrumentID);
	if (iterFound != m_symbols.end())
	{
		longlong timestamp = boost::chrono::steady_clock::now().time_since_epoch().count();
		m_pCallback->OnQuoteReceived(pDepthMarketData, timestamp);
	}
}
