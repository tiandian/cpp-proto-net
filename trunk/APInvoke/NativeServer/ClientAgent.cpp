#include "StdAfx.h"
#include "ClientAgent.h"
#include "../Transmission/gen/entity.pb.h"

#include <iostream>

ClientAgent::ClientAgent(void):
m_session(NULL)
{
}


ClientAgent::~ClientAgent(void)
{
}

void ClientAgent::OnCallbackResponse( const string& method, const string& return_data )
{
	if(method == "TestCallback")
	{
		Entity::TestCallbackReturn cbReturn;
		cbReturn.ParseFromString(return_data);
		int retVal = cbReturn.retval();
		std::cout << "Return value from client: " << retVal << std::endl;
	}
	else
	{
		std::cout << "Unexpected callback response method: " << method << std::endl;
	}
}

void ClientAgent::DoCallback()
{
	Entity::TestCallbackParams callbackParams;
	
	callbackParams.set_param3("xixi");
	callbackParams.set_param4("haha");
	std::string callbackData;
	callbackParams.SerializeToString(&callbackData);

	m_session->BeginCallback("TestCallback", callbackData);
}
