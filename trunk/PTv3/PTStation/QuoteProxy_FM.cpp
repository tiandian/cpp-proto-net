#include "StdAfx.h"
#include "QuoteProxy_FM.h"
#include "MdSpi_FM.h"
#include "FileOperations.h"
#include "QuoteAggregator.h"

#include <boost/algorithm/string.hpp>
#include <boost/shared_array.hpp>

const int TOPIC = 100;

void RunQuoteProc(CUstpFtdcMduserApi* pUserApi, string connAddress)
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
	, m_isRunning(false)
	, m_isUdp(false)
{
}

CQuoteProxy::~CQuoteProxy(void)
{
}

bool CQuoteProxy::Begin()
{
	try
	{
		cout << m_investorId << " Connecting " << m_connAddr << endl; 

		unsigned ipEndPos = m_connAddr.find_last_of(":");
		m_connectingIP = m_connAddr.substr(6, ipEndPos - 6);
		string flowFolder = m_investorId + "/Md-" + m_connectingIP;

		if(!CreateFolderIfNotExists(flowFolder))
		{
			cerr << "Cannot create stream folder (" << flowFolder << ")" << endl;
			return false;
		}
		flowFolder += "/";

		m_isUdp = boost::istarts_with(m_connAddr, "udp");

		// ´´½¨UserApi
		m_pUserApi = CUstpFtdcMduserApi::CreateFtdcMduserApi(flowFolder.c_str());
		assert(m_pUserApi != NULL);
		
		m_pMdSpi = boost::shared_ptr<CMdSpi>(new CMdSpi(this));
		m_pUserApi->RegisterSpi(m_pMdSpi.get());
		m_pUserApi->SubscribeMarketDataTopic(TOPIC, USTP_TERT_QUICK);

		string connAddress;
		if (m_isUdp)
		{
			// Populate udp address with prefix tcp://
			connAddress = boost::ireplace_first_copy(m_connAddr, "udp", "tcp");
		}
		else
		{
			connAddress = m_connAddr;
		}

		m_thQuoting = boost::thread(boost::bind(&RunQuoteProc, m_pUserApi, connAddress));

		if (m_isUdp)
		{
			m_udpListener = UdpQuoteListenerPtr(
				new CUdpQuoteListener(41115,
				boost::bind(&CQuoteProxy::OnUdpDataReceived, this, _1, _2)));
		}

		m_isRunning = true;
		return true;
	}
	catch (exception& e)
	{
		cerr << "Cannot start QuoteProxy (" << m_investorId << "-" << m_connectingIP << ") due to " << e.what() << endl;
		return false;
	}
	
}

int CQuoteProxy::WaitUntilEnd()
{
	m_thQuoting.join();
	return m_isReady ? 0 : -1; // if it was ever ready, consider it as normal exit
}

void CQuoteProxy::End()
{
	m_isRunning = false;
	if(m_pUserApi != NULL)
	{
		m_pUserApi->Release();
		m_pUserApi = NULL;
	}
	
	if(!m_isReady)
	{
		// if not ready until going to end, subscribeMarketData must be waiting for ready
		GetReady();
	}
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
		if(m_isRunning)
		{
			cout << m_investorId << "-" << m_connAddr << " Subscribing " << symCount << " symbol(s). The first is " << symbolArr[0] << endl;
			int iResult = m_pUserApi->SubMarketData(symbolArr, symCount);
		}
	}
}

void CQuoteProxy::UnsubscribeMarketData( char** symbolArr, int symCount )
{
	if(symbolArr != NULL && symCount > 0 && m_isRunning)
	{
		cout << m_investorId << "-" << m_connAddr << " Un-Subscribing " << symCount << " symbol(s). The first is " << symbolArr[0] << endl;
		int iResult = m_pUserApi->UnSubMarketData(symbolArr, symCount);
	}
}

void CQuoteProxy::OnQuoteReceived( CUstpFtdcDepthMarketDataField *pDepthMarketData )
{
	if(!m_isUdp)
		m_quoteAggregator->OnQuoteReceived(m_connectingIP, pDepthMarketData);
}

void CQuoteProxy::OnUdpDataReceived(char* pData, std::size_t nSize)
{
	CUstpFtdcDepthMarketDataField *pDepthMarketData = reinterpret_cast<CUstpFtdcDepthMarketDataField*>(pData);
	m_quoteAggregator->OnQuoteReceived(m_connectingIP, pDepthMarketData);
}
