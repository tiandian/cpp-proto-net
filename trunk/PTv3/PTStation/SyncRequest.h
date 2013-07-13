#pragma once

#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace std;

template<typename T>
class CSyncRequest
{
public:
	CSyncRequest(int nRequestId, int timeout = 5):
		  m_nRequestId(nRequestId), m_invokeSucc(false), m_timeoutSec(timeout) {}

	~CSyncRequest(void){}

	template<typename Handler>
	bool Invoke(Handler handler)
	{
		return handler(m_nRequestId);
	}

	void SetResult(bool succ, T* pResult)
	{
		boost::mutex::scoped_lock lock(m_mut);
		m_invokeSucc = succ;
		m_result = pResult;
		m_condWaitRes.notify_one();
	}

	bool GetResult(T** ppResult)
	{
		boost::unique_lock<boost::mutex> lock(m_mut);
		if(m_condWaitRes.timed_wait(lock, boost::posix_time::seconds(m_timeoutSec)))
		{
			*ppResult = m_result;
			return m_invokeSucc;
		}
		// invoke time out
		*ppResult = NULL;
		return false;
	}

private:
	T* m_result;
	bool m_invokeSucc;
	int m_nRequestId;
	int m_timeoutSec;
	boost::condition_variable m_condWaitRes;
	boost::mutex m_mut;

};

template<typename T>
class CSyncRequestFactory
{
public:
	CSyncRequestFactory(){}

	~CSyncRequestFactory(){}

	boost::shared_ptr< CSyncRequest<T> > Create(int nRequestId)
	{
		boost::mutex::scoped_lock lock(m_mut);
		boost::shared_ptr< CSyncRequest<T> > req(new CSyncRequest<T>(nRequestId));
		m_requesterMap.insert(make_pair(nRequestId, req));
		return req;
	}

	void Remove(int nRequestId)
	{
		boost::mutex::scoped_lock lock(m_mut);
		m_requesterMap.erase(nRequestId);
	}

	void Response(int nReqestId, bool succ, T* pResult)
	{
		boost::mutex::scoped_lock lock(m_mut);
		typename map<int, boost::shared_ptr< CSyncRequest<T> > >::iterator iter = 
			m_requesterMap.find(nReqestId);
		if(iter != m_requesterMap.end())
		{
			iter->second->SetResult(succ, pResult);
		}
	}

private:
	map<int, boost::shared_ptr< CSyncRequest<T> > > m_requesterMap;
	boost::mutex m_mut;
};






