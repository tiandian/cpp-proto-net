
#include "QS_Configuration.h"
#include "MdSpi.h"

#include <iostream>
#include <cassert>
#include <boost/shared_array.hpp>

#ifdef WIN32
#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")
#endif

using namespace std;

CQSConfiguration qsConfig;

int main(int argc, char* argv[])
{
	if(!qsConfig.Load(argc, argv))
	{
		cout << "Invalid or missing arguments" << endl;
		return 1;
	}

	cout << "Hell QuoteStation" << endl;
	cout << "Connection string: " << qsConfig.ConnectionString() << endl;
	cout << "BrokerId: " << qsConfig.BrokerId() << endl;
	cout << "Username: " << qsConfig.Username() << endl;
	cout << "Password: " << qsConfig.Password() << endl;

	// 创建UserApi
	CThostFtdcMdApi* pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
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