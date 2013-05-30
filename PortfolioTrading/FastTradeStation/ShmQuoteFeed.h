#pragma once

#include "shm_remover.h"
#include "ThostTraderApi/ThostFtdcUserApiStruct.h"

#include <string>

#include <boost/thread.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

using namespace std;
using namespace boost::interprocess;

#define SHM_QUOTE_FEED_NAME "QuoteFeed_1-"

#ifdef WIN32
#define longlong __int64
#endif

struct MarketDataObj
{  
	MarketDataObj()  
		: running(false)
		, ready(false)
	{
		clear();
	}

	void clear()
	{
		memset(&mktDataField, 0, sizeof(mktDataField));
	}

	//Mutex to protect access to the object  
	boost::interprocess::interprocess_mutex      mutex;  

	//Condition to wait when the object is ready for subscribe or unsubscribe  
	boost::interprocess::interprocess_condition  cond_ready;  

	bool	ready;
	bool	running;

	CThostFtdcDepthMarketDataField mktDataField;
	longlong timestamp;
};  

class CShmQuoteFeedProducer
{
public:
	CShmQuoteFeedProducer(const string& shmName)
		: m_shmName(shmName), m_pData(NULL){}
	~CShmQuoteFeedProducer(){}

	bool Init();
	void Put(CThostFtdcDepthMarketDataField* quoteData);

private:
	string m_shmName;
	shared_memory_object m_shm;
	mapped_region m_region;

	MarketDataObj* m_pData;
};

typedef boost::function<void(CThostFtdcDepthMarketDataField*, longlong)> PushQuoteFunc;

class CShmQuoteFeedConsumer
{
public:
	CShmQuoteFeedConsumer(const string& shmName, PushQuoteFunc pushQuoteFunc) 
		: m_shmName(shmName), m_remover(shmName), m_pData(NULL)
		, m_pushQuoteFunc(pushQuoteFunc){}
	~CShmQuoteFeedConsumer()
	{
		if(m_pData != NULL)
		{
			scoped_lock<interprocess_mutex> lock(m_pData->mutex);
			m_pData->running = false;
			m_pData->cond_ready.notify_one();
		}
		m_thread.join();
		m_pData = NULL;
	}

	bool Init();
	void Start();

private:
	void PopProc();

	string m_shmName;
	shm_remove m_remover;
	shared_memory_object m_shm;
	mapped_region m_region;

	MarketDataObj* m_pData;
	boost::thread m_thread;
	PushQuoteFunc m_pushQuoteFunc;
};

