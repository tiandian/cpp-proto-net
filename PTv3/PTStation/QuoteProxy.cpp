#include "StdAfx.h"
#include "QuoteProxy.h"
#include "MdSpi.h"
#include "FileOperations.h"
#include "QuoteAggregator.h"

#include <boost/algorithm/string.hpp>
#include <boost/shared_array.hpp>

void RunQuoteProc(CThostFtdcMdApi* pUserApi, string connAddress)
{
	int conStrLen = connAddress.length();
	boost::shared_array<char> FRONT_ADDR(new char[conStrLen + 1]);
	strncpy(FRONT_ADDR.get(), connAddress.c_str(), conStrLen);
	pUserApi->RegisterFront(FRONT_ADDR.get());	
	pUserApi->Init();

	pUserApi->Join();
}

CQuoteProxy::CQuoteProxy(CQuoteAggregator* pQuoteAggregator, const string& connAddr, const string& brokerId, const string& investorId, const string& passwd)
	: m_quoteAggregator(pQuoteAggregator)
	, m_pUserApi(NULL)
	, m_connAddr(connAddr)
	, m_brokerId(brokerId)
	, m_investorId(investorId)
	, m_passwd(passwd)
	, m_isReady(false)
{
}

CQuoteProxy::~CQuoteProxy(void)
{
}

bool CQuoteProxy::Begin()
{
	try
	{
		unsigned ipEndPos = m_connAddr.find_last_of(":");
		m_connectingIP = m_connAddr.substr(6, ipEndPos - 6);
		string flowFolder = m_investorId + "/Md-" + m_connectingIP;

		if(!CreateFolderIfNotExists(flowFolder))
		{
			cerr << "Cannot create stream folder (" << flowFolder << ")" << endl;
			return false;
		}
		flowFolder += "/";

		bool isUdp = boost::istarts_with(m_connAddr, "udp");

		// ´´½¨UserApi
		m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi(flowFolder.c_str(), isUdp);
		assert(m_pUserApi != NULL);

		m_pMdSpi = boost::shared_ptr<CMdSpi>(new CMdSpi(this));
		m_pUserApi->RegisterSpi(m_pMdSpi.get());

		string connAddress;
		if(isUdp)
		{
			// Populate udp address with prefix tcp://
			connAddress = boost::ireplace_first_copy(m_connAddr, "udp", "tcp");
		}
		else
		{
			connAddress = m_connAddr;
		}

		m_thQuoting = boost::thread(boost::bind(&RunQuoteProc, m_pUserApi, connAddress));

		return true;
	}
	catch (exception& e)
	{
		cerr << "Cannot start QuoteProxy (" << m_investorId << "-" << m_connectingIP << ") due to " << e.what() << endl;
		return false;
	}
	
}

bool CQuoteProxy::WaitUntilEnd()
{
	m_thQuoting.join();
	return m_isReady ? 0 : -1;
}

void CQuoteProxy::End()
{
	m_pUserApi->Release();
}


void CQuoteProxy::GetReady()
{
	boost::unique_lock<boost::mutex> l(m_mutex);
	m_isReady = true;
	m_readyEvent.notify_one();
}

void CQuoteProxy::SubscribeMarketData( char** symbolArr, int symCount )
{
	if(symbolArr != NULL && symCount > 0)
	{
		boost::unique_lock<boost::mutex> l(m_mutex);
		m_readyEvent.wait(l, boost::bind(&CQuoteProxy::IsReady, this));

		cout << m_investorId << "-" << m_connAddr << " Subscribing " << symCount << " symbol(s). The first is " << symbolArr[0] << endl;
		int iResult = m_pUserApi->SubscribeMarketData(symbolArr, symCount);
	}
}

void CQuoteProxy::UnsubscribeMarketData( char** symbolArr, int symCount )
{
	if(symbolArr != NULL && symCount > 0)
	{
		cout << m_investorId << "-" << m_connAddr << " Unsubscribing " << symCount << " symbol(s). The first is " << symbolArr[0] << endl;
		int iResult = m_pUserApi->UnSubscribeMarketData(symbolArr, symCount);
	}
}

void CQuoteProxy::OnQuoteReceived( CThostFtdcDepthMarketDataField *pDepthMarketData )
{
	m_quoteAggregator->OnQuoteReceived(m_connectingIP, pDepthMarketData);
}
