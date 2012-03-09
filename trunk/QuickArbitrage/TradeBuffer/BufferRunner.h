#pragma once

#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>

#define BUFFER_DEFAULT_CAPACITY 128

template < typename T >
class CBufferRunner
{
public:
	CBufferRunner(boost::function<void(T)> callback):
	  m_jobCallback(callback), m_isRunning(false), m_hasStuff(false)
	{
		SetCapacity(BUFFER_DEFAULT_CAPACITY);
	}
	~CBufferRunner(void)
	{
	}

	void SetCapacity(int capacity)
	{
		m_cbQuotes.set_capacity(capacity);
	}

	void Start()
	{
		m_isRunning = true;
		m_thread = boost::thread(boost::bind(&CBufferRunner::Dequeue, this));
	}

	void Stop()
	{
		m_isRunning = false;
		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			m_cond.notify_all();
		}
	}

	void Enqueue(T stuff)
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_cbQuotes.push_back(stuff);
		m_cond.notify_all();
	}

private:

	void Dequeue()
	{
		while(m_isRunning)
		{
			boost::unique_lock<boost::mutex> lock(m_mutex);
			while (m_cbQuotes.size() == 0 && m_isRunning)
			{
				m_cond.wait(lock);         
			}

			T& stuff = m_cbQuotes.front();

			m_jobCallback(stuff);
			
			m_cbQuotes.pop_front();
		}  
	}



	boost::condition_variable m_cond;
	boost::mutex m_mutex;
	bool m_isRunning;
	bool m_hasStuff;

	boost::circular_buffer< T > m_cbQuotes;

	boost::function<void(T&)> m_jobCallback;
	boost::thread m_thread;
};

