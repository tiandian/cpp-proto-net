
#include "QS_Configuration.h"
#include "SubscribeQuoteObj.h"
#include "MdSpi.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <csignal>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/shared_array.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#ifdef WIN32
#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")
#endif

using namespace std;
using namespace boost::interprocess;

CQSConfiguration qsConfig;
CThostFtdcMdApi* pUserApi = NULL;

int launchChildTest(int argc, char* argv[]);
void signalHandler( int signum );
void subscribeQuoteProc(SubscribeQuoteObj * data);

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

	//Erase previous shared memory and schedule erasure on exit  
	struct shm_remove  
	{  
		shm_remove(const string& name) : _name(name)
		{ shared_memory_object::remove(_name.c_str()); }  
		~shm_remove()
		{ shared_memory_object::remove(_name.c_str()); }  
		string _name;
	} remover(shmName);

	//Create a shared memory object.  
	shared_memory_object shm  
		(create_only               //only create  
		,shmName.c_str()           //name  
		,read_write                //read-write mode  
		);

	try{  
		//Set size  
		shm.truncate(sizeof(SubscribeQuoteObj));

		//Map the whole shared memory in this process  
		mapped_region region  
			(shm                    //What to map  
			,read_write				//Map it as read-write
			);  

		//Get the address of the mapped region  
		void * addr       = region.get_address();  

		//Construct the shared structure in memory  
		SubscribeQuoteObj * data = new (addr) SubscribeQuoteObj;
		data->running = true;
		boost::thread th(boost::bind(&subscribeQuoteProc, data));
	  
		string cmd = sCmd.str();
		cout << "Launch Child : " << cmd << endl;
		int ret = system(cmd.c_str());
		cout << "Child process exit " << ret << endl;
		return ret;

	}  
	catch(interprocess_exception &ex){  
		std::cout << ex.what() << std::endl;  
		return 1;  
	}
}

void subscribeQuoteProc(SubscribeQuoteObj * data)
{
	try{ 
		for(int i = 0; i < 5; ++i)
		{
			boost::this_thread::sleep_for(boost::chrono::seconds(15));

			{
				scoped_lock<interprocess_mutex> lock(data->mutex);
				if(data->running)
				{
					if(data->message_in)
					{
						data->cond_ready.wait(lock);
					}

					if(i < 4)
					{
						strcpy(data->items[0], "IF1305");
						if(i % 2 == 0)
							data->subscribe = true;
						else
							data->subscribe = false;
						data->message_in = true;
					}
					else
					{
						// notify proc subscribe quote exit
						data->running = false;
					}
					
					data->cond_submit.notify_one();
				}
			}
		}
	}
	catch(interprocess_exception &ex){  
		std::cout << ex.what() << std::endl;  
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