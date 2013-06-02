#pragma once

#include "../APInvokeNative/APInvokeNative.h"
#include "ClientAgent.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

class ServerSessionCallback : public SessionManagerHandler
{
public:
	ServerSessionCallback(void);
	~ServerSessionCallback(void);

	virtual void OnConnected(Session* session);

	virtual void OnDisconnected(Session* session);

	virtual void OnError(Session* session, const string& errorMsg);

	virtual void DispatchPacket(const string& sessionId, 
		const string& method, const string& in_data, string& out_data);

private:
	typedef boost::shared_ptr<ClientAgent> ClientPtr;

	typedef std::map<std::string, ClientPtr>::iterator ClientMapIter;
	std::map<std::string, ClientPtr> m_clientsMap;
	
};

