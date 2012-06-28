#pragma once

#include "../../APInvoke/APInvokeNative/APInvokeNative.h"
#include "ClientAgent.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

using namespace std;

typedef boost::shared_ptr<CClientAgent> ClientPtr;
typedef map<string, ClientPtr> ClientMap;
typedef ClientMap::iterator ClientMapIter;

class CClientManager : public SessionManagerHandler
{
public:
	CClientManager(void);
	~CClientManager(void);

	virtual void OnConnected(Session* session);

	virtual void OnDisconnected(Session* session);

	virtual void OnError(Session* session, const string& errorMsg);

	virtual void DispatchPacket(const string& sessionId, 
		const string& method, const string& in_data, string& out_data);

private:
	ClientMap m_clients;
};

