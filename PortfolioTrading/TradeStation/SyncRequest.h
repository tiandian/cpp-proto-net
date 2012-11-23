#pragma once

#include <map>
#include <boost/shared_ptr.hpp>

using namespace std;

class CSyncRequestFactory;

class CSyncRequest
{
public:
	CSyncRequest(int nRequestId, CSyncRequestFactory* factory);
	~CSyncRequest(void);

private:
	int m_nRequestId;
	CSyncRequestFactory* m_factory;
};

typedef boost::shared_ptr<CSyncRequest> SyncRequestPtr;
typedef map<int, SyncRequestPtr> RequesterMap;
typedef RequesterMap::iterator RequesterMapIter;

class CSyncRequestFactory
{
public:
	CSyncRequestFactory(){}

	~CSyncRequestFactory(){}

	SyncRequestPtr Create(int nRequestId)
	{
		SyncRequestPtr req(new CSyncRequest(nRequestId, this));
		m_requesterMap.insert(make_pair(nRequestId, req));
		return req;
	}

	void Remove(int nRequestId);

private:
	RequesterMap m_requesterMap;
};






