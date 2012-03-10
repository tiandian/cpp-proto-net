// TradeBuffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Configuration.h"
#include "LogManager.h"
#include "ConnectionManager.h"
#include "MarketAgent.h"
#include "QuoteAggregator.h"
#include "OrderManager.h"
#include "ConsoleClient.h"

#include <iostream>
#include <string>
#include <boost/thread/locks.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
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
void ConsoleExecuteSubscribe(CConsoleClient* pConsole, string& cmd);
void ConsoleExecuteUnSubscribe(CConsoleClient* pConsole);

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

	g_connMgr.Listen(config.GetPort());
	
	if(ctrl_type == CONSOLE)
	{
		CConsoleClient consoleClient;

		bool exit = false;
		while(!exit)
		{
			string command;
			cout << ">";
			std::getline(cin, command);
			//cin >> command;
			boost::to_lower(command);
			if(command == "q" || command == "quit")
			{
				exit = true;
				cout << "Exit." << endl;
			}
			else if(command == "h" || command == "help")
			{
				PrintHelp();
			}
			else if(boost::istarts_with(command, "subscribe"))
			{
				ConsoleExecuteSubscribe(&consoleClient, command);
			}
			else if(boost::istarts_with(command, "unsub"))
			{
				ConsoleExecuteUnSubscribe(&consoleClient);
			}
			else if(boost::istarts_with(command, "unsuball"))
			{
				ConsoleExecuteUnSubscribe(&consoleClient);
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
	cout << "Allowed commands are following" << endl;
	cout << "	q or quit" << endl;
	cout << "	subscribe [symbol] ..." << endl;
	cout << "	unsub" << endl;

}

void Cleanup()
{
	cout << "do some clean up before quit" << endl;
	g_marketAgent.Logout(config.GetBrokerID(), config.GetInvestorID());
	g_marketAgent.Disconnect();
}

void OutputSubcribedSymbol(CConsoleClient* pConsole)
{
	vector<string>& symbols = pConsole->GetSymbols();
	BOOST_FOREACH(string s, symbols)
	{
		cout << s << ", ";
	}
	cout << endl;
}

void ConsoleExecuteSubscribe(CConsoleClient* pConsole, string& cmd)
{
	typedef vector< string > split_vector_type;

	split_vector_type splitVec; 
	boost::split( splitVec, cmd, boost::is_any_of(" "), boost::token_compress_on );

	if(splitVec.size() > 1)
	{
		splitVec.erase(splitVec.begin()); // remove the first;
		pConsole->Subscribe( splitVec );
	}
	else
	{
		OutputSubcribedSymbol(pConsole);
	}
}

void ConsoleExecuteUnSubscribe(CConsoleClient* pConsole)
{
	pConsole->UnSubscribe();
}