#include "StdAfx.h"
#include "ConnectionManager.h"
#include "LogManager.h"

#include <sstream>
#include <boost/bind.hpp>

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

}

