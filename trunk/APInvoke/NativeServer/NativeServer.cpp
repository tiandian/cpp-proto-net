// NativeServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServerSessionCallback.h"
#include "../APInvokeNative/APInvokeNative.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <google/protobuf/stubs/common.h>

#pragma comment(lib, "../APInvokeNative/lib/APInvokeNative.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	ServerSessionCallback sessionCallback;
	boost::shared_ptr<SessionManager> pSessionManager(SessionManager::Create());
	pSessionManager->RegisterHandler(&sessionCallback);
	pSessionManager->Listen(16888);

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

