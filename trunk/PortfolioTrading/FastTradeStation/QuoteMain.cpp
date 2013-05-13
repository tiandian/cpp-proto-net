
#include "QS_Configuration.h"
#include "ShmQuoteSubscribe.h"
#include "MdSpi.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <csignal>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/shared_array.hpp>


#ifdef WIN32
#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")
#endif

using namespace std;
using namespace boost::interprocess;

CQSConfiguration qsConfig;
CThostFtdcMdApi* pUserApi = NULL;

int launchChildTest(int argc, char* argv[]);
void signalHandler( int signum );
void subscribeQuoteProc(CShmQuoteSubscribeProducer * producer);

int main(int argc, char* argv[])
{
	if(!qsConfig.Load(argc, argv))
	{
		cout << "Invalid or missing arguments" << endl;
		return 1;
	}

	if(qsConfig.TestHost())
	{
		return launchChildTest(argc, argv);
	}

	cout << "Hell QuoteStation" << endl;
	cout << "Connection string: " << qsConfig.ConnectionString() << endl;
	cout << "BrokerId: " << qsConfig.BrokerId() << endl;
	cout << "Username: " << qsConfig.Username() << endl;
	cout << "Password: " << qsConfig.Password() << endl;

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	// 创建UserApi
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
	assert(pUserApi != NULL);

	CMdSpi mdSpiImpl(pUserApi);
	// 注册事件类
	pUserApi->RegisterSpi(&mdSpiImpl);
	// connect
	int conStrLen = qsConfig.ConnectionString().length();
	boost::shared_array<char> FRONT_ADDR(new char[conStrLen + 1]);
	strncpy(FRONT_ADDR.get(), qsConfig.ConnectionString().c_str(), conStrLen);
	pUserApi->RegisterFront(FRONT_ADDR.get());	
	pUserApi->Init();

	pUserApi->Join();

	return 0;
}

int launchChildTest(int argc, char* argv[])
{
	cout << "This is a testing host, which will launch a child for test" << endl;
	stringstream sCmd;
	assert(argc == 11);
	for (int i = 0; i < 9; ++i)
	{
		if(i > 0)
			sCmd << " ";
		sCmd << argv[i];
	}
	sCmd << ends;

	string shmName = "SubscribeQuote-" + qsConfig.BrokerId() + "-" + qsConfig.Username();
	CShmQuoteSubscribeProducer producer(shmName);
	producer.Init();

	boost::thread th(boost::bind(&subscribeQuoteProc, &producer));

	string cmd = sCmd.str();
	cout << "Launch Child : " << cmd << endl;
	int ret = system(cmd.c_str());
	cout << "Child process exit " << ret << endl;
	return ret;

}

void subscribeQuoteProc(CShmQuoteSubscribeProducer * producer)
{
	vector<string> symbols;
	symbols.push_back("IF1305");
	symbols.push_back("IF1306");

	for(int i = 0; i < 4; ++i)
	{
		boost::this_thread::sleep_for(boost::chrono::seconds(15));
		if(i < 4)
		{
			if(i % 2 == 0)
				producer->Put(symbols, true);
			else
				producer->Put(symbols, false);
		}
	}
	cout << "Test subscribeQuoteProc done." << endl;
}

void signalHandler( int signum )
{
	cout << "signal (" << signum << ") received.\n";

	switch(signum)
	{
	case SIGINT:
		cout << "Interrupt received" << endl;
		if(pUserApi != NULL)
			pUserApi->Release();
		break;
	case SIGTERM:
		cout << "Terminate received" << endl;
		if(pUserApi != NULL)
			pUserApi->Release();
		break;
	}
	// cleanup and close up stuff here  
	// terminate program  

	//exit(signum);  

}