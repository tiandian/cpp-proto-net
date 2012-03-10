#pragma once

#include "server.h"

#include <boost/smart_ptr.hpp>

class CConnectionManager
{
public:
	CConnectionManager(void);
	~CConnectionManager(void);

	void Listen(unsigned int nPort);
	void Stop();

private:

	void OnClientAccepted(connection_ptr conn);
	
	boost::shared_ptr<server> m_server;

};

