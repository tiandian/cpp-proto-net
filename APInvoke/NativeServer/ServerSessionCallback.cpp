#include "StdAfx.h"
#include "ServerSessionCallback.h"
#include <iostream>

ServerSessionCallback::ServerSessionCallback(void)
{
}


ServerSessionCallback::~ServerSessionCallback(void)
{
}

void ServerSessionCallback::OnConnected( Session* session )
{
	std::cout << session->SessionId();
}

void ServerSessionCallback::DispatchPacket( const string& sessionId, const string& method, const string& in_data, string& out_data )
{

}

void ServerSessionCallback::OnDisconnected( Session* session )
{

}

void ServerSessionCallback::OnError( Session* session, const string& errorMsg )
{

}
