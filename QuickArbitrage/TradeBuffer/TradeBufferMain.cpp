// TradeBuffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Configuration.h"
#include "LogManager.h"
#include "ConnectionManager.h"
#include "MarketAgent.h"
#include "QuoteAggregator.h"
#include "OrderManager.h"

#include <iostream>
#include <string>
#include <boost/thread/locks.hpp>
#include <google/protobuf/stubs/common.h>

#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")

using namespace std;

//////////////////////////////////////////////////////////////////////////
// Create global objects here
CConfiguration config;
CLogManager	logger;
CConnectionManager g_connMgr;
CMarketAgent g_marketAgent;
CQuoteAggregator g_quoteAggregator;
COrderManager g_orderMgr;

//////////////////////////////////////////////////////////////////////////

void PrintHelp();
void Cleanup(); 

boost::condition_variable _condExit;
boost::mutex _mut;

int _tmain(int argc, _TCHAR* argv[])
{
	if(!config.Load(argc, argv))
		return 1;

	string ctrl_type = config.GetControlType();
	if(!(ctrl_type == REMOTE || ctrl_type == CONSOLE))
	{
		cerr << "Given unknown control type ('" << ctrl_type << "'), please set '" << CONSOLE << "' or '" << REMOTE << "'" << endl;
		return 1;
	}

	logger.Init();

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	if(!g_marketAgent.Connect())
		return 1;

	if(!g_marketAgent.Login(config.GetBrokerID(), config.GetInvestorID(), config.GetPassword()))
	{
		Cleanup();
		return 1;
	}

	g_quoteAggregator.Initialize(&g_marketAgent);
	g_orderMgr.Initialize();
	
	if(ctrl_type == CONSOLE)
	{
		bool exit = false;
		while(!exit)
		{
			string command;
			cout << ">";
			cin >> command;
			if(command == "q" || command == "quit")
			{
				exit = true;
				cout << "Exit." << endl;
			}
			else if(command == "h" || command == "help")
			{
				PrintHelp();
			}
			else
			{
				cout << "Unknown command '" << command << "', please type 'h' or 'help' for help" << endl;
			}
		}
	}
	else
	{
		boost::unique_lock< boost::mutex > lock(_mut);
		_condExit.wait(lock);
	}

	Cleanup();

	return 0;
}

void ExitProgramFromRemote()
{
	_condExit.notify_one();
}

void PrintHelp()
{
	cout << "here is help" << endl;
}

void Cleanup()
{
	cout << "do some clean up before quit" << endl;
	g_marketAgent.Logout(config.GetBrokerID(), config.GetInvestorID());
	g_marketAgent.Disconnect();
}
