#include "ClientManager.h"
#include "LogManager.h"
#include "Configuration.h"

#include <iostream>
#include <string>
#include <boost/format.hpp> 
#include <boost/algorithm/string.hpp>
#include <google/protobuf/stubs/common.h>
#include <iostream>
#include <google/protobuf/stubs/common.h>

#define FST_VERSION "1.1"

using namespace std;

CConfiguration config;
CLogManager	logger;

CClientManager g_ClientManager;

int main(int argc, char* argv[])
{
	cout << "Fast Trade Station v" << FST_VERSION << endl;

	if(!config.Load(argc, argv))
		return 1;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	logger.Init();

	boost::shared_ptr<SessionManager> pSessionManager(SessionManager::Create());
	pSessionManager->RegisterHandler(&g_ClientManager);
	const string& sAddr = config.GetAddr();
	const string& sPort = config.GetPort();
	pSessionManager->Listen(sAddr, sPort);
	logger.Info(boost::str(boost::format("Start listening at %s:%s") 
		% (sAddr.empty() ? "localhost" : sAddr.c_str()) % sPort.c_str()));

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