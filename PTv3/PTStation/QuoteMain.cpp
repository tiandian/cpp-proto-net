#include "StdAfx.h"
#include "QS_Configuration.h"
#include "QS_LogManager.h"
#include "ShmQuoteSubscribe.h"
#include "ShmQuoteFeed.h"
#include "MdSpi.h"
#include "FileOperations.h"
#include "QuoteProxy.h"
#include "QuoteAggregator.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <csignal>
#include <set>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/shared_array.hpp>
#include <boost/algorithm/string.hpp>

#ifdef WIN32

#ifndef USE_FEMAS_API
#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")
#else
#pragma comment(lib, "./FemasAPI/USTPmduserapi.lib")
#endif

#endif

using namespace std;
using namespace boost::interprocess;

enum {
	INVALID_STARTUP_ARGUMENTS = 1,
	BEGIN_QUOTE_PROXY_ERROR = 2,
	QUOTE_AGGREGATOR_INITIALIZATION_ERROR = 3,
	QUOTE_PROXY_UNEXPECTED_EXIT = 4
};

CQSConfiguration qsConfig;
CQSLogManager* pLogger = NULL;
vector<QuoteProxyPtr> quoteProxyVec;

int launchChildTest(int argc, char* argv[]);
void signalHandler( int signum );
void subscribeQuoteProc(CShmQuoteSubscribeProducer * producer);

#ifndef USE_FEMAS_API
void OnQuotePush(CThostFtdcDepthMarketDataField* mktDataField);
#else
void OnQuotePush(CUstpFtdcDepthMarketDataField* mktDataField);
#endif

void OnSubscribeMarketData(char** symbolArr, int symCount);
void OnUnsubscribeMarketData(char** symbolArr, int symCount);
void OnTerminateNotified();

int main(int argc, char* argv[])
{
	if(!qsConfig.Load(argc, argv))
	{
		cout << "Invalid or missing arguments" << endl;
		return INVALID_STARTUP_ARGUMENTS;
	}

	if(qsConfig.TestHost())
	{
		return launchChildTest(argc, argv);
	}

	CQSLogManager qsLogger;
	pLogger = &qsLogger;
	if(qsConfig.LogQuote())
	{
		qsLogger.Init();
	}

	cout << "Startup QuoteStation v4.0.0";
#ifndef USE_FEMAS_API
	cout << endl;
#else
	cout << " for Femas" << endl;
#endif
	
	cout << "Connection string: " << qsConfig.ConnectionString() << endl;
	cout << "BrokerId: " << qsConfig.BrokerId() << endl;
	cout << "Username: " << qsConfig.Username() << endl;
	cout << "Password: " << qsConfig.Password() << endl;
	cout << "Logging quote: " << (qsConfig.LogQuote() ? "Yes" : "No") << endl;

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	CQuoteAggregator quoteAggregator(
		boost::bind(&OnSubscribeMarketData,  _1, _2), 
		boost::bind(&OnUnsubscribeMarketData, _1, _2), 
		boost::bind(&OnTerminateNotified));

	/*
	const string& connections = qsConfig.ConnectionString();
	boost::split( mktDataSources, connections, boost::is_any_of(","), boost::token_compress_on );
	*/
#ifdef _DEBUG
	vector< string > mktDataSources;
	mktDataSources.push_back(qsConfig.ConnectionString());
	//mktDataSources.push_back("udp://192.168.205.3:18213"); This is not available, just for test ever
	for(vector<string>::iterator iter = mktDataSources.begin(); iter != mktDataSources.end(); ++iter)
#else
	set< string > mktDataSources;
	mktDataSources.insert("tcp://172.21.5.12:41213");
	mktDataSources.insert("udp://192.168.163.61:18213");
	mktDataSources.insert("udp://192.168.205.3:18213");
	mktDataSources.insert("tcp://192.168.217.4:41213");
	mktDataSources.insert("tcp://192.168.217.5:41213");

	mktDataSources.insert(qsConfig.ConnectionString());

	for(set<string>::iterator iter = mktDataSources.begin(); iter != mktDataSources.end(); ++iter)
#endif
	{
		QuoteProxyPtr quoteProxyMain(QuoteProxyPtr(new CQuoteProxy(&quoteAggregator,
			*iter, qsConfig.BrokerId(), qsConfig.Username(), qsConfig.Password())));
		if(!quoteProxyMain->Begin())
			return BEGIN_QUOTE_PROXY_ERROR;
		quoteProxyVec.push_back(quoteProxyMain);
	}

	if(!quoteAggregator.Initialize(qsConfig.BrokerId(), qsConfig.Username()))
		return QUOTE_AGGREGATOR_INITIALIZATION_ERROR;

	int proxyExitCode = 0;
	// Wait until all quote proxy end successfully
	for(vector<QuoteProxyPtr>::iterator iter = quoteProxyVec.begin();
		iter != quoteProxyVec.end(); ++iter)
	{
		int errCd = (*iter)->WaitUntilEnd();
		if(errCd != 0)
			proxyExitCode = errCd;
	}

	return proxyExitCode;
}

void OnSubscribeMarketData(char** symbolArr, int symCount)
{
	for(vector<QuoteProxyPtr>::iterator iter = quoteProxyVec.begin();
		iter != quoteProxyVec.end(); ++iter)
	{
		(*iter)->SubscribeMarketData(symbolArr, symCount);
	}
}

void OnUnsubscribeMarketData(char** symbolArr, int symCount)
{
	for(vector<QuoteProxyPtr>::iterator iter = quoteProxyVec.begin();
		iter != quoteProxyVec.end(); ++iter)
	{
		(*iter)->UnsubscribeMarketData(symbolArr, symCount);
	}
}

void OnTerminateNotified()
{
	for(vector<QuoteProxyPtr>::iterator iter = quoteProxyVec.begin();
		iter != quoteProxyVec.end(); ++iter)
	{
		(*iter)->End();
	}
}

int launchChildTest(int argc, char* argv[])
{
	cout << "This is a testing host, which will launch a child for test" << endl;
	stringstream sCmd;
	//assert(argc == 11);
	for (int i = 0; i < 11; ++i)
	{
		if(i > 0)
			sCmd << " ";
		sCmd << argv[i];
	}
	sCmd << ends;

	string shmName = SHM_SUBSCRIBE_NAME + qsConfig.BrokerId() + "-" + qsConfig.Username();
	CShmQuoteSubscribeProducer producer(shmName);
	producer.Init();

	string quoteFeedName = SHM_QUOTE_FEED_NAME + qsConfig.BrokerId() + "-" + qsConfig.Username();
	CShmQuoteFeedConsumer feedee(quoteFeedName, boost::bind(&OnQuotePush, _1));
	feedee.Init();
	feedee.Start();

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
	symbols.push_back("IF1501");
	//symbols.push_back("IF1306");

	for(int i = 0; i < 5; ++i)
	{
		boost::this_thread::sleep_for(boost::chrono::seconds(15));
		if(i < 4)
		{
			if(i % 2 == 0)
				producer->Put(symbols, true);
			else
				producer->Put(symbols, false);
		}
		else
			producer->NotifyTerminate();
	}
	cout << "Test subscribeQuoteProc done." << endl;
}

#ifndef USE_FEMAS_API
void OnQuotePush(CThostFtdcDepthMarketDataField* mktDataField)
#else
void OnQuotePush(CUstpFtdcDepthMarketDataField* mktDataField)
#endif
{
	cout << "[Parent process] OnQuotePush : " << mktDataField->InstrumentID << ", "
		<< mktDataField->LastPrice << ", "
		<< mktDataField->UpdateTime << ", "
		<< mktDataField->UpdateMillisec << endl;
}

void signalHandler( int signum )
{
	cout << "signal (" << signum << ") received.\n";

	switch(signum)
	{
	case SIGINT:
		cout << "Interrupt received" << endl;
		OnTerminateNotified();
		break;
	case SIGTERM:
		cout << "Terminate received" << endl;
		OnTerminateNotified();
		break;
	}
	// cleanup and close up stuff here  
	// terminate program  

	//exit(signum);  

}