#include "StdAfx.h"
#include "APSessionManager.h"

SessionManager* SessionManager::Create()
{
	return new APSessionManager;
}

APSessionManager::APSessionManager(void):
m_callback(NULL)
{
}


APSessionManager::~APSessionManager(void)
{
}

bool APSessionManager::Listen( unsigned int nPort )
{
	m_server = boost::shared_ptr<server>(new server(nPort));
	m_server->set_accept_handler(boost::bind(&APSessionManager::OnClientAccepted, this, _1));

	if(m_callback != NULL)
		m_callback->OnConnected(NULL);
	return true;
}

void APSessionManager::Close()
{

}

void APSessionManager::RegisterCallback( SessionManagerHandler* handler )
{
	m_callback = handler;
}

void APSessionManager::OnClientAccepted( connection_ptr conn )
{

}
