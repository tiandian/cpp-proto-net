#include "ShmQuoteSubscribe.h"

#include <cassert>
#include <boost/shared_array.hpp>

#ifndef WIN32
#define strcpy_s strcpy
#endif

bool CShmQuoteSubscribeProducer::Init()
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
		m_shm.truncate(sizeof(SubscribeQuoteObj));

		//Map the whole shared memory in this process  
		m_region = mapped_region  
			(m_shm						//What to map  
			,read_write					//Map it as read-write
			);  

		//Get the address of the mapped region  
		void * addr = m_region.get_address();  

		//Construct the shared structure in memory  
		m_pData = new (addr) SubscribeQuoteObj;
		m_pData->running = true;
		return m_pData != NULL;
	}
	catch (interprocess_exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}
}

void CShmQuoteSubscribeProducer::Put(vector<string>& symbols, bool subscribe)
{
	assert(m_pData != NULL);

	scoped_lock<interprocess_mutex> lock(m_pData->mutex);
	if(m_pData->running)
	{
		if(!m_pData->ready)
		{
			m_pData->cond_ready.wait(lock);
		}

		int count = symbols.size();
		count = count > SubscribeQuoteObj::MaxCount ? SubscribeQuoteObj::MaxCount : count;

		for (int i = 0; i < count; ++i)
		{
			strcpy_s(m_pData->items[i], symbols[i].c_str());
		}

		m_pData->count = count;
		m_pData->subscribe = subscribe;
		m_pData->ready = false;
		m_pData->cond_submit.notify_one();
	}
}

void CShmQuoteSubscribeProducer::NotifyTerminate()
{
	if(m_pData != NULL)
	{
		scoped_lock<interprocess_mutex> lock(m_pData->mutex);
		m_pData->running = false;
		m_pData->cond_submit.notify_one();
	}
}

bool CShmQuoteSubscribeConsumer::Init()
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
		m_pData = static_cast<SubscribeQuoteObj*>(addr);  

		return m_pData != NULL;
	}
	catch (interprocess_exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}
}

void CShmQuoteSubscribeConsumer::Start()
{
	assert(m_pData != NULL);

	m_thread = boost::thread(boost::bind(&CShmQuoteSubscribeConsumer::GetProc, this));
}

void CShmQuoteSubscribeConsumer::GetProc()
{
	bool end_loop = false;  
	do{  
		scoped_lock<interprocess_mutex> lock(m_pData->mutex);
		
		if(m_pData->ready)
			m_pData->cond_submit.wait(lock);

		if(m_pData->running)
		{
			if(m_pData->count > 0)
			{
				boost::shared_array<char*> symbols( new char*[m_pData->count] );
				for(int i = 0; i < m_pData->count; ++i)
				{
					symbols[i] = m_pData->items[i];
				}

				if(m_pData->subscribe)
				{
					if(!m_subscribeSymbolFunc.empty())
						m_subscribeSymbolFunc(symbols.get(), m_pData->count);
				}
				else
				{
					if(!m_unsubSymbolFunc.empty())
						m_unsubSymbolFunc(symbols.get(), m_pData->count);
				}
			}
			
			m_pData->clear();
			m_pData->ready = true;
			m_pData->cond_ready.notify_one();
		}
		else
			end_loop = true;
	}  
	while(!end_loop);

	if(!m_onTerminateFunc.empty())
		m_onTerminateFunc();
}

