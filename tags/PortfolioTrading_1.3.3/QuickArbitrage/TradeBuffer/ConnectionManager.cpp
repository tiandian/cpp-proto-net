#include "StdAfx.h"
#include "ConnectionManager.h"
#include "LogManager.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


extern CLogManager	logger;

CConnectionManager::CConnectionManager(void)
{
}


CConnectionManager::~CConnectionManager(void)
{
}

void CConnectionManager::Listen( unsigned int nPort )
{
	m_server = boost::shared_ptr<server>(new server(nPort));
	m_server->set_accept_handler(boost::bind(&CConnectionManager::OnClientAccepted, this, _1));
	
	std::ostringstream info;
	info << "Begin listening on port " << nPort;
	logger.Info(info.str());
}

void CConnectionManager::Stop()
{
	// close all client first
	m_clientMap.clear();

	// destory server
	m_server.reset();
	logger.Info("Stop quoting service.");
}

void CConnectionManager::OnClientAccepted( connection_ptr conn )
{
	boost::uuids::random_generator gen;
	boost::uuids::uuid uuid_ = gen();
	std:string sessionId = boost::uuids::to_string(uuid_);
	
	RemoteClientPtr client(new RemoteClient(sessionId, conn));
	m_clientMap.insert(std::make_pair(sessionId, client));
	client->GetReady(this);
}

void CConnectionManager::HandleError( const std::string sessionId, const boost::system::error_code& e )
{
	if(e)
	{
		// log error
		std::ostringstream oss;
		std::map<std::string, RemoteClientPtr>::iterator foundClnt = m_clientMap.find(sessionId);
		if(foundClnt != m_clientMap.end())
		{
			oss << "Client (ip:" << (foundClnt->second)->GetIPAddress() << ", sid:" << sessionId <<") encounter error:";
			oss << e.message();
		}
		else
		{
			oss << "Error happent to the not existing client : " << e.message();
		}
		logger.Warning(oss.str());

		// handle error
		int eVal = e.value();

		// client close the connection
		if(eVal == 10054)
		{
			// close myself
			(foundClnt->second)->Close();
			// remove from map storage
			m_clientMap.erase(foundClnt);
		}
	}
}

void CConnectionManager::ListClients()
{
	std::ostringstream info;
	int idx = 1;
	for (std::map<std::string, RemoteClientPtr>::iterator iter = m_clientMap.begin();
		iter != m_clientMap.end(); ++iter, ++idx)
	{
		info << std::endl;
		info << std::setw(4) << idx << std::setw(20) << (iter->second)->GetIPAddress();
		info << "   " << (iter->second)->GetSessionID();
	}
	logger.Info(info.str());
}

