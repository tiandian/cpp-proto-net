#include "StdAfx.h"
#include "ShmQuoteFeed.h"

#include <boost/chrono.hpp>

bool CShmQuoteFeedConsumer::Init()
{
	try
	{
		//Create a shared memory object.  
		m_shm = shared_memory_object  
			(create_only               //only create  
			,m_shmName.c_str()           //name  
			,read_write                //read-write mode  
			);

		//Set size  
		m_shm.truncate(sizeof(MarketDataObj));

		//Map the whole shared memory in this process  
		m_region = mapped_region  
			(m_shm						//What to map  
			,read_write					//Map it as read-write
			);  

		//Get the address of the mapped region  
		void * addr = m_region.get_address();  

		//Construct the shared structure in memory  
		m_pData = new (addr) MarketDataObj;
		m_pData->running = true;
		return m_pData != NULL;
	}
	catch (interprocess_exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}
}

void CShmQuoteFeedConsumer::Start()
{
	assert(m_pData != NULL);

	m_thread = boost::thread(boost::bind(&CShmQuoteFeedConsumer::PopProc, this));
}

void CShmQuoteFeedConsumer::PopProc()
{
	bool end_loop = false;  
	do{  
		scoped_lock<interprocess_mutex> lock(m_pData->mutex);

		if(!m_pData->ready)
			m_pData->cond_ready.wait(lock);

		if(m_pData->running)
		{
			if(!m_pushQuoteFunc.empty())
			{
				m_pushQuoteFunc(&(m_pData->mktDataField), m_pData->timestamp);
			}

			m_pData->ready = false;
		}
		else
			end_loop = true;
	}  
	while(!end_loop);
}

bool CShmQuoteFeedProducer::Init()
{
	try
	{
		//Create a shared memory object.  
		m_shm = shared_memory_object  
			(open_only                    //only create  
			,m_shmName.c_str()              //name  
			,read_write                   //read-write mode  
			);  
		//Map the whole shared memory in this process  
		m_region = mapped_region  
			(m_shm                       //What to map  
			,read_write //Map it as read-write  
			);  

		//Get the address of the mapped region  
		void * addr       = m_region.get_address();  

		//Obtain a pointer to the shared structure  
		m_pData = static_cast<MarketDataObj*>(addr);  

		return m_pData != NULL;
	}
	catch (interprocess_exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}
}

void CShmQuoteFeedProducer::Put( CThostFtdcDepthMarketDataField* quoteData )
{
	assert(m_pData != NULL);

	scoped_lock<interprocess_mutex> lock(m_pData->mutex);
	if(m_pData->running)
	{
		memcpy(&(m_pData->mktDataField), quoteData, sizeof(CThostFtdcDepthMarketDataField));
		m_pData->timestamp = boost::chrono::steady_clock::now().time_since_epoch().count();
		m_pData->ready = true;
		m_pData->cond_ready.notify_one();
	}
}
