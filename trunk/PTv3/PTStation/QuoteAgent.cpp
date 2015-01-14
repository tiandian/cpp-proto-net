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
#ifdef FAKE_QUOTE
	, m_pushingFakeQuote(false)
#endif
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
#ifdef FAKE_QUOTE
	m_pushingFakeQuote = false;
#endif
}

void CQuoteAgent::LaunchChildProc(string cmd)
{
	m_retChild = system(cmd.c_str());
	LOG_DEBUG(logger, boost::str(boost::format("Child Quote process returned with %d") % m_retChild));

	m_bIsConnected.store(false, boost::memory_order_release);
}

boost::tuple<bool, string> CQuoteAgent::Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
#ifndef USE_FEMAS_API
	string logonId = investorId;
#else
	string logonId = userId;
#endif
	
	string traceInfo = boost::str(boost::format("Log in quote (%s, %s, %s)") 
		% brokerId.c_str() % logonId % password);
	logger.Trace(traceInfo);

	m_brokerID = brokerId;
	m_userID = logonId;

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
	//sCmd << " -q true";
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
#ifdef FAKE_QUOTE
	m_pushingFakeQuote = true;
	m_thFakeQuote = boost::thread(boost::bind(&CQuoteAgent::PushFakeQuote, this));
#endif
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

void CQuoteAgent::PushFakeQuote()
{
	if (m_pCallback != NULL)
	{
		ostringstream timeStream;
		boost::posix_time::time_facet* const timeFormat = new boost::posix_time::time_facet("%H:%M:%S");
		timeStream.imbue(std::locale(timeStream.getloc(), timeFormat));

		while (true)
		{
			boost::this_thread::sleep(boost::posix_time::seconds(1));
#ifndef USE_FEMAS_API
			CThostFtdcDepthMarketDataField mktDataField;
#else
			CUstpFtdcDepthMarketDataField mktDataField;
#endif
			memset(&mktDataField, 0, sizeof(mktDataField));

			strcpy(mktDataField.InstrumentID, "IF1501");
			mktDataField.LastPrice = 3200;
			mktDataField.AskPrice1 = 3201;
			mktDataField.BidPrice1 = 3200;

			const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
			timeStream << now;
			string timeText = timeStream.str();
			strcpy_s(mktDataField.UpdateTime, timeText.c_str());
			timeStream.str("");
			timeStream.clear();

			longlong timestamp = boost::chrono::steady_clock::now().time_since_epoch().count();
			m_pCallback->OnQuoteReceived(&mktDataField, timestamp);
		}
	}
}

