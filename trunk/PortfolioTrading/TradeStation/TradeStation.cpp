// TradeStation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ClientManager.h"

#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <google/protobuf/stubs/common.h>

using namespace std;

#ifdef _DEBUG
#pragma comment(lib, "../Debug/APInvokeNative.lib")
#else
#pragma comment(lib, "../Release/APInvokeNative.lib")
#endif

CClientManager g_ClientManager;

int _tmain(int argc, _TCHAR* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	boost::shared_ptr<SessionManager> pSessionManager(SessionManager::Create());
	pSessionManager->RegisterHandler(&g_ClientManager);
	pSessionManager->Listen(16168);

	bool exit = false;
	while(!exit)
	{
		string command;
		cout << ">";
		getline(cin, command);
		//cin >> command;
		boost::to_lower(command);
		if(command == "q" || command == "quit")
		{
			exit = true;
		}
	}

	cout << "Close Sessions" << endl;
	pSessionManager->Close();

	cout << "Exit." << endl;

	return 0;
}

