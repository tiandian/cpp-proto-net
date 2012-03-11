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
	m_server->stop();
	m_server.reset();
}

void CConnectionManager::OnClientAccepted( connection_ptr conn )
{
	boost::uuids::random_generator gen;
	boost::uuids::uuid uuid_ = gen();
	std:string sessionId = boost::uuids::to_string(uuid_);
	
	RemoteClientPtr client(new RemoteClient(sessionId, conn));
	m_clientMap.insert(std::make_pair(sessionId, client));
	client->GetReady();
}

