#include "StdAfx.h"
#include "SyncRequest.h"


CSyncRequest::CSyncRequest(int nRequestId, CSyncRequestFactory* factory):
m_nRequestId(nRequestId),
m_factory(factory)
{
}


CSyncRequest::~CSyncRequest(void)
{
	if(m_factory != NULL)
	{
		m_factory->Remove(m_nRequestId);
		m_factory = NULL;
	}
}

//SyncRequestPtr CSyncRequestFactory::Create( int nRequestId )
//{
//	SyncRequestPtr req(new CSyncRequest(nRequestId, this));
//	m_requesterMap.insert(make_pair(nRequestId, req));
//	return req;
//}

void CSyncRequestFactory::Remove( int nRequestId )
{
	m_requesterMap.erase(nRequestId);
}
