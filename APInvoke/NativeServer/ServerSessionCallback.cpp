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
	std::cout << session->SessionId() << " Connected." << std::endl;
}

void ServerSessionCallback::DispatchPacket( const string& sessionId, const string& method, const string& in_data, string& out_data )
{

}

void ServerSessionCallback::OnDisconnected( Session* session )
{
	std::cout << session->SessionId() << " Disconnected." << std::endl;
}

void ServerSessionCallback::OnError( Session* session, const string& errorMsg )
{
	std::cout << session->SessionId() << " has error:" << errorMsg << std::endl;
}
