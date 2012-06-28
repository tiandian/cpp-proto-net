#include "StdAfx.h"
#include "ClientManager.h"


CClientManager::CClientManager(void)
{
}


CClientManager::~CClientManager(void)
{
}

void CClientManager::OnConnected( Session* session )
{

}

void CClientManager::OnDisconnected( Session* session )
{

}

void CClientManager::OnError( Session* session, const string& errorMsg )
{

}

void CClientManager::DispatchPacket( const string& sessionId, const string& method, const string& in_data, string& out_data )
{

}
