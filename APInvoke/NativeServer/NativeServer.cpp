// NativeServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServerSessionCallback.h"
#include "../APInvokeNative/APInvokeNative.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <google/protobuf/stubs/common.h>


#pragma comment(lib, "../APInvokeNative/lib/APInvokeNative.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 2)
	{
		std::cerr << "Port is not specified" << std::endl;
		return 1;
	}

	int port = boost::lexical_cast<int>(argv[1]);
	std::cout << "Start listening at port " << port << "..." << std::endl;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	ServerSessionCallback sessionCallback;
	boost::shared_ptr<SessionManager> pSessionManager(SessionManager::Create());
	pSessionManager->RegisterHandler(&sessionCallback);
	pSessionManager->Listen(port);

	bool exit = false;
	while(!exit)
	{
		string command;
		std::cout << ">";
		std::getline(cin, command);
		//cin >> command;
		boost::to_lower(command);
		if(command == "q" || command == "quit")
		{
			exit = true;
		}
	}

	std::cout << "Close Sessions" << std::endl;
	pSessionManager->Close();
	//delete pSessionManager;

	std::cout << "Exit." << std::endl;

	return 0;
}

