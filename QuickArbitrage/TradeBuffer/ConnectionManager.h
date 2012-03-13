#pragma once

#include "server.h"
#include "RemoteClient.h"

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>

typedef boost::shared_ptr<RemoteClient> RemoteClientPtr;

class CConnectionManager
{
public:
	CConnectionManager(void);
	~CConnectionManager(void);

	void Listen(unsigned int nPort);
	void Stop();

	void ListClients();

	void HandleError(const std::string sessionId, const boost::system::error_code& e);

private:

	void OnClientAccepted(connection_ptr conn);
	
	boost::shared_ptr<server> m_server;

	std::map<std::string, RemoteClientPtr> m_clientMap;
};

