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
void ConsoleExecuteLogin(CConsoleClient& console, string& cmd);
void ConsoleExecuteLogout(CConsoleClient& console);
void ConsoleExecuteBuy(CConsoleClient& console, string& cmd);

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

	g_connMgr.Listen(config.GetPort());

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	if(!g_marketAgent.Connect())
	{
		Cleanup();
		return 1;
	}
	if(!g_marketAgent.Login(config.GetBrokerID(), config.GetInvestorID(), config.GetPassword()))
	{
		Cleanup();
		return 1;
	}

	g_quoteAggregator.Initialize(&g_marketAgent);
	g_orderMgr.Initialize();
	
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
			else if(boost::istarts_with(command, "login"))
			{
				ConsoleExecuteLogin(consoleClient, command);
			}
			else if(command == "logout")
			{
				ConsoleExecuteLogout(consoleClient);
			}
			else if(boost::istarts_with(command, "buy"))
			{
				ConsoleExecuteBuy(consoleClient, command);
			}
			else if(command == "list")
			{
				g_connMgr.ListClients();
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
	cout << "   list" << endl;

}

void Cleanup()
{
	g_marketAgent.Logout(config.GetBrokerID(), config.GetInvestorID());
	g_marketAgent.Disconnect();
	g_connMgr.Stop();
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

void ConsoleExecuteLogin(CConsoleClient& console, string& cmd)
{
	typedef vector< string > split_vector_type;

	split_vector_type splitVec; 
	boost::split( splitVec, cmd, boost::is_any_of(" "), boost::token_compress_on );

	if(splitVec.size() == 3)
	{
		string& uid = splitVec[1];
		string& pwd = splitVec[2];
		string brokerId = "0240";
		bool succ = console.Login(brokerId, uid, pwd);

		cout << "Console client logged in successfully." << endl;
	}
	else
	{
		cout << "Invalid login arguments. [e.g. login [account] [password]]" << endl;
		cout << "account could be 0240050002 - 0240050009, password could be 888888" << endl;
	}
}

void ConsoleExecuteLogout(CConsoleClient& console)
{
	console.Logout();
}

void ConsoleExecuteBuy(CConsoleClient& console, string& cmd)
{
	console.Buy();
}