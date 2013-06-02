#include "StdAfx.h"
#include "ConnectionTester.h"

#include <boost/smart_ptr.hpp>

#define TEST_CONN_TIMEOUT 15

CConnectionTester::CConnectionTester(void):
m_pUserApi(NULL)
{
}


CConnectionTester::~CConnectionTester(void)
{
}

void CConnectionTester::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_condConnectDone.notify_one();
	
}

void CConnectionTester::OnFrontDisconnected( int nReason )
{

}

bool CConnectionTester::Validate( const std::string& address, int* delay )
{
	bool succ = false;

	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(strdup(address.c_str()));

	m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\Md\\");
	m_pUserApi->RegisterSpi(this);	
	
	m_thConnect = boost::thread(boost::bind(&CConnectionTester::ConnectMarketFunc, this, front_addr.get()));

	// wait 30 seconds for connected event
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);
		if(m_condConnectDone.timed_wait(lock, boost::posix_time::seconds(TEST_CONN_TIMEOUT)))
		{
			succ = true;
		}
	}

	m_pUserApi->Release();
	m_thConnect.join();

	return succ;
}

void CConnectionTester::ConnectMarketFunc(char* pAddress)
{
	m_pUserApi->RegisterFront(pAddress);					// connect
	m_pUserApi->Init();

	m_pUserApi->Join();
}
