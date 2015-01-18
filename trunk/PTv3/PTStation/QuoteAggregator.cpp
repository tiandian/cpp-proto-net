#include "StdAfx.h"
#include "QuoteAggregator.h"
#include "QS_LogManager.h"

#include <exception>
#include <boost/chrono.hpp>

#ifndef WIN32
#define strcpy_s strcpy
#endif

extern CQSLogManager* pLogger;

CQuoteAggregator::CQuoteAggregator(OnSubscribeQuoteFunc subscribeFunc, OnSubscribeQuoteFunc unsubFunc, OnQuotingEndFunc quoteEndFunc)
	: m_subscribeFunc(subscribeFunc)
	, m_unsubscribeFunc(unsubFunc)
	, m_quotingEndFunc(quoteEndFunc)
{
#ifdef USE_FEMAS_API
	memset(&IntermediateMarketData, 0, sizeof(IntermediateMarketData));
#endif
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
	if(!m_subscribeFunc.empty())
		m_subscribeFunc(symbolArr, symCount);
}

void CQuoteAggregator::UnsubscribeMarketData( char** symbolArr, int symCount )
{
	if(!m_subscribeFunc.empty())
		m_unsubscribeFunc(symbolArr, symCount);
}

void CQuoteAggregator::OnTerminateNotified()
{
	if(!m_quotingEndFunc.empty())
		m_quotingEndFunc();
}

#ifdef USE_FEMAS_API
void CopyMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CUstpFtdcDepthMarketDataField *pUstpDepthMarketData)
{
	strcpy_s(pDepthMarketData->InstrumentID, pUstpDepthMarketData->InstrumentID);
	strcpy_s(pDepthMarketData->TradingDay, pUstpDepthMarketData->TradingDay);
	strcpy_s(pDepthMarketData->UpdateTime, pUstpDepthMarketData->UpdateTime);

	pDepthMarketData->LastPrice = pUstpDepthMarketData->LastPrice;
	pDepthMarketData->BidPrice1 = pUstpDepthMarketData->BidPrice1;
	pDepthMarketData->BidVolume1 = pUstpDepthMarketData->BidVolume1;
	pDepthMarketData->AskPrice1 = pUstpDepthMarketData->AskPrice1;
	pDepthMarketData->AskVolume1 = pUstpDepthMarketData->AskVolume1;

	pDepthMarketData->UpdateMillisec = pUstpDepthMarketData->UpdateMillisec;

	pDepthMarketData->PreSettlementPrice = pUstpDepthMarketData->PreSettlementPrice;
	pDepthMarketData->PreClosePrice = pUstpDepthMarketData->PreClosePrice;
	pDepthMarketData->PreOpenInterest = pUstpDepthMarketData->PreOpenInterest;
	pDepthMarketData->OpenPrice = pUstpDepthMarketData->OpenPrice;
	pDepthMarketData->HighestPrice = pUstpDepthMarketData->HighestPrice;
	pDepthMarketData->LowestPrice = pUstpDepthMarketData->LowestPrice;
	pDepthMarketData->Volume = pUstpDepthMarketData->Volume;
	pDepthMarketData->Turnover = pUstpDepthMarketData->Turnover;;
	pDepthMarketData->OpenInterest = pUstpDepthMarketData->OpenInterest;
	pDepthMarketData->ClosePrice = pUstpDepthMarketData->ClosePrice;
	pDepthMarketData->SettlementPrice = pUstpDepthMarketData->SettlementPrice;
	pDepthMarketData->UpperLimitPrice = pUstpDepthMarketData->UpperLimitPrice;
	pDepthMarketData->LowerLimitPrice = pUstpDepthMarketData->LowerLimitPrice;
	pDepthMarketData->PreDelta = pUstpDepthMarketData->PreDelta;
	pDepthMarketData->CurrDelta = pUstpDepthMarketData->CurrDelta;
}
#endif

#ifndef USE_FEMAS_API
void CQuoteAggregator::OnQuoteReceived( const string& connectIP, CThostFtdcDepthMarketDataField *pDepthMarketData )
#else
void CQuoteAggregator::OnQuoteReceived(const string& connectIP, CUstpFtdcDepthMarketDataField *pUstpDepthMarketData)
#endif
{
	boost::mutex::scoped_lock l(m_mutex);

#ifdef USE_FEMAS_API
	CopyMarketData(&IntermediateMarketData, pUstpDepthMarketData);
	CThostFtdcDepthMarketDataField *pDepthMarketData = &IntermediateMarketData;
#endif

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

	if (delay == 0)
	{
		pLogger->Info(boost::str(boost::format("Quote(%s): %s\t%d\t%.1f\t%.1f\t%.1f\t%.1f")
			% symbol % pDepthMarketData->UpdateTime % pDepthMarketData->UpdateMillisec 
			% pDepthMarketData->OpenPrice % pDepthMarketData->HighestPrice % pDepthMarketData->LowestPrice % pDepthMarketData->LastPrice));
	}

	pLogger->Info(boost::str(boost::format("%-15s\t%s\t%s\t%d\t%d")
		% connectIP % symbol % pDepthMarketData->UpdateTime % pDepthMarketData->UpdateMillisec % delay));
}


