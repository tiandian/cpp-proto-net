#include "StdAfx.h"
#include "ServerSessionCallback.h"
#include "..\Transmission\gen\entity.pb.h"

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
	ClientPtr client(new ClientAgent);
	client->SetSession(session);
	m_clientsMap.insert(std::make_pair(session->SessionId(), client));
}

void ServerSessionCallback::DispatchPacket( const string& sessionId, const string& method, const string& in_data, string& out_data )
{
	static bool retVal = false;
	std::cout << "Invocation to " << method << " coming in with (" << sessionId << ")" << std::endl;
	if(method == "TestHello")
	{
		Entity::TestHelloParams helloParam;
		helloParam.ParseFromString(in_data);
		std::cout << "Param1: " << helloParam.param1() << "\tParam2: " << helloParam.param2() << std::endl;

		ClientMapIter iter = m_clientsMap.find(sessionId);
		if(iter != m_clientsMap.end())
		{
			ClientPtr clnt = iter->second;
			clnt->DoCallback();
		}

		Entity::TestHelloReturn helloReturn;
		helloReturn.set_success(retVal);
		helloReturn.SerializeToString(&out_data);

		retVal = !retVal;
	}
	else
	{
		std::cerr << "Unexpected method: " << method << std::endl;
	}
}

void ServerSessionCallback::OnDisconnected( Session* session )
{
	std::cout << session->SessionId() << " Disconnected." << std::endl;
}

void ServerSessionCallback::OnError( Session* session, const string& errorMsg )
{
	std::cout << session->SessionId() << " has error:" << errorMsg << std::endl;
}
