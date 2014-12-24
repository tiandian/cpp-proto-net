#include "StdAfx.h"
#include "QuoteAgent.h"
#include "globalmembers.h"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

CQuoteAgent::CQuoteAgent(void)
	: m_pCallback(NULL)
	, m_bIsConnected(false)
	, m_retChild(-1)
{
}


CQuoteAgent::~CQuoteAgent(void)
{
	// if connected, notify Quotestation for termination and wait for end, otherwise do nothing
	if(m_bIsConnected.load(boost::memory_order_acquire))
	{
		Logout();
		m_thLaunch.join();
	}
}

void CQuoteAgent::LaunchChildProc(string cmd)
{
	m_retChild = system(cmd.c_str());
	LOG_DEBUG(logger, boost::str(boost::format("Child Quote process returned with %d") % m_retChild));

	m_bIsConnected.store(false, boost::memory_order_release);
}

boost::tuple<bool, string> CQuoteAgent::Login( const string& frontAddr, const string& brokerId, const string& userId, const string& password )
{
	string traceInfo = boost::str(boost::format("Log in quote (%s, %s, %s)") 
		% brokerId.c_str() % userId % password);
	logger.Trace(traceInfo);

	m_brokerID = brokerId;
	m_userID = userId;

	string shmName = SHM_SUBSCRIBE_NAME + m_brokerID + "-" + m_userID;
	m_quoteSubscriber = boost::shared_ptr<CShmQuoteSubscribeProducer>( new CShmQuoteSubscribeProducer(shmName) );
	bool initSucc = m_quoteSubscriber->Init();
	if(!initSucc)
		return boost::make_tuple(false, "Quote subscriber initialization failed");

	string quoteFeedName = SHM_QUOTE_FEED_NAME + m_brokerID + "-" + m_userID;
	m_quoteFeedee = boost::shared_ptr<CShmQuoteFeedConsumer>( new CShmQuoteFeedConsumer(quoteFeedName,
		boost::bind(&CQuoteAgent::OnQuotePush, this, _1, _2)) );
	initSucc = m_quoteFeedee->Init();
	if(!initSucc)
		return boost::make_tuple(false, "Quote feedee initialization failed");

	m_quoteFeedee->Start();

	stringstream sCmd;
#ifndef USE_FEMAS_API
	sCmd << "QuoteStation";
#else
	sCmd << "QuoteStationFM";
#endif
	sCmd << " -c " << frontAddr; //tcp://ctpsim-front01.gfqh.cn:43213
	sCmd << " -b " << m_brokerID;
	sCmd << " -u " << m_userID;
	sCmd << " -p " << password;
	sCmd << " -q true";
	string cmd = sCmd.str();

	logger.Info(boost::str(boost::format("Launch Child with %s") % cmd));
	m_thLaunch = boost::thread(boost::bind(&CQuoteAgent::LaunchChildProc, this, cmd));

	// wait 15 seconds for QuoteStation initialization
	bool isSubscriberReady = m_quoteSubscriber->GetReady(15);
	m_bIsConnected.store(isSubscriberReady, boost::memory_order_relaxed);

	return boost::make_tuple(isSubscriberReady, isSubscriberReady ? "" : "QuoteStation is not ready yet");
}

void CQuoteAgent::Logout()
{
	m_quoteSubscriber->NotifyTerminate();
}

bool CQuoteAgent::SubscribesQuotes( vector<string>& subscribeArr )
{
	if(!m_bIsConnected.load(boost::memory_order_consume))
		return false;

	bool retVal = false;

	try	
	{
		string symbolsToSub = boost::join(subscribeArr, ", ");
		LOG_INFO(logger, boost::str(boost::format("Subscribe quote for symbols as following %s") % symbolsToSub));

		m_quoteSubscriber->Put(subscribeArr, true);

		retVal = true;
	}
	catch(...)
	{
		logger.Error("Unknown error happened while subscribe market data for quotation");
	}

	return retVal;
}

bool CQuoteAgent::UnSubscribesQuotes( vector<string>& unSubscribeArr )
{
	if(!m_bIsConnected.load(boost::memory_order_consume))
		return false;

	bool retVal = false;

	try	
	{
		string symbolsToUnSub = boost::join(unSubscribeArr, ", ");
		LOG_INFO(logger, boost::str(boost::format("UnSubscribe quote for symbols as following %s") % symbolsToUnSub));

		m_quoteSubscriber->Put(unSubscribeArr, false);

		retVal = true;
	}
	catch(...)
	{
		logger.Error("Unknown error happened while Unsubscribe market data for quotation");
	}

	return retVal;
}

#ifndef USE_FEMAS_API
void CQuoteAgent::OnQuotePush(CThostFtdcDepthMarketDataField* mktDataField, longlong timestamp)
#else
void CQuoteAgent::OnQuotePush(CUstpFtdcDepthMarketDataField* mktDataField, longlong timestamp)
#endif
{
	if(m_pCallback != NULL)
	{
		m_pCallback->OnQuoteReceived(mktDataField, timestamp);
	}
}

