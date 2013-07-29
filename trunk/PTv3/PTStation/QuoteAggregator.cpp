#include "StdAfx.h"
#include "QuoteAggregator.h"
#include "QS_LogManager.h"

#include <exception>
#include <boost/chrono.hpp>

extern CQSLogManager* pLogger;

CQuoteAggregator::CQuoteAggregator(OnSubscribeQuoteFunc subscribeFunc, OnSubscribeQuoteFunc unsubFunc, OnQuotingEndFunc quoteEndFunc)
	: m_subscribeFunc(subscribeFunc)
	, m_unsubscribeFunc(unsubFunc)
	, m_quotingEndFunc(quoteEndFunc)
{
}

CQuoteAggregator::~CQuoteAggregator(void)
{
}

bool CQuoteAggregator::Initialize(const string& brokerId, const string& userId)
{
	try{
		string shmName = SHM_SUBSCRIBE_NAME + brokerId + "-" + userId;
		cout << "Opening shm " << shmName << " for quote subscribe" << endl;
		m_quoteSubscriber = boost::shared_ptr<CShmQuoteSubscribeConsumer>
			( new CShmQuoteSubscribeConsumer(shmName,
			boost::bind(&CQuoteAggregator::SubscribeMarketData, this, _1, _2),
			boost::bind(&CQuoteAggregator::UnsubscribeMarketData, this, _1, _2),
			boost::bind(&CQuoteAggregator::OnTerminateNotified, this)));
		bool initSucc = m_quoteSubscriber->Init();
		if(!initSucc)
		{
			cout << "[QuoteStation] Quote subscriber initialization failed" << endl;
			return false;
		}

		string quoteFeedName = SHM_QUOTE_FEED_NAME + brokerId + "-" + userId;
		cout << "Open shm " << quoteFeedName << " for quote feeding" << endl;
		m_quoteFeeder = boost::shared_ptr<CShmQuoteFeedProducer>( new CShmQuoteFeedProducer(quoteFeedName));
		initSucc = m_quoteFeeder->Init();
		if(!initSucc)
		{
			cout << "[QuoteStation] Quote feeder initialization failed" << endl;
			return false;
		}

		m_quoteSubscriber->Start();
		return true;
	}
	catch(exception& e)
	{
		cerr << "QuotoAggregator Initialization Error :" << e.what() << endl;
	}
	return false;
}

void CQuoteAggregator::SubscribeMarketData( char** symbolArr, int symCount )
{
	m_subscribeFunc(symbolArr, symCount);
}

void CQuoteAggregator::UnsubscribeMarketData( char** symbolArr, int symCount )
{
	m_unsubscribeFunc(symbolArr, symCount);
}

void CQuoteAggregator::OnTerminateNotified()
{
	m_quotingEndFunc();
}

void CQuoteAggregator::OnQuoteReceived( const string& connectIP, CThostFtdcDepthMarketDataField *pDepthMarketData )
{
	boost::mutex::scoped_lock l(m_mutex);
	long delay = 0;
	string symbol = pDepthMarketData->InstrumentID;
	QuoteTimestampMapIter iter = m_lastQuoteTimestamp.find(symbol);
	if(iter == m_lastQuoteTimestamp.end())
	{
		TimestampPtr ts(new QuoteTimestamp(pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec));
		ts->LastTime = boost::chrono::steady_clock::now();
		m_lastQuoteTimestamp.insert(make_pair(symbol, ts));
		m_quoteFeeder->Put(pDepthMarketData);
	}
	else
	{
		if(iter->second->UpdateTime == pDepthMarketData->UpdateTime &&
			iter->second->MilliSeconds == pDepthMarketData->UpdateMillisec)
		{
			boost::chrono::steady_clock::duration elapsed = boost::chrono::steady_clock::now() - iter->second->LastTime;
			delay = boost::chrono::duration_cast<boost::chrono::microseconds>(elapsed).count();
		}
		else
		{
			iter->second->UpdateTime = pDepthMarketData->UpdateTime;
			iter->second->MilliSeconds = pDepthMarketData->UpdateMillisec;
			iter->second->LastTime = boost::chrono::steady_clock::now();
			m_quoteFeeder->Put(pDepthMarketData);
		}
	}

	pLogger->Info(boost::str(boost::format("%s\t%s\t%s\t%d\t%d")
		% connectIP % symbol % pDepthMarketData->UpdateTime % pDepthMarketData->UpdateMillisec % delay));
}


