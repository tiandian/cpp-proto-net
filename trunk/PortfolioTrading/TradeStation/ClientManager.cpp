#include "StdAfx.h"
#include "ClientManager.h"
#include "globalmembers.h"

#include <boost/format.hpp>

CClientManager::CClientManager(void)
{
}


CClientManager::~CClientManager(void)
{
}

void CClientManager::OnConnected( Session* session )
{
	std::string info = boost::str(boost::format("Client(%s) connected.") % session->SessionId().c_str());
	logger.Info(info);
}

void CClientManager::OnDisconnected( Session* session )
{
	std::string info = boost::str(boost::format("Client(%s) disconnected.") % session->SessionId().c_str());
	logger.Info(info);
}

void CClientManager::OnError( Session* session, const string& errorMsg )
{
	std::string outputErr = boost::str(boost::format("Client(%s) encounter error - %s.") 
		% session->SessionId().c_str() % errorMsg.c_str());
	logger.Info(outputErr);
}

void CClientManager::DispatchPacket( const string& sessionId, const string& method, const string& in_data, string& out_data )
{

}
