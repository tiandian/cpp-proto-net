#include "StdAfx.h"
#include "ConsoleClient.h"
#include "LogManager.h"
#include "OrderManager.h"

extern CLogManager logger;
extern COrderManager g_orderMgr;

CConsoleClient::CConsoleClient(void):
	m_streamOut(std::stringstream::out), m_pRemoteClient(NULL)
{
}


CConsoleClient::~CConsoleClient(void)
{
}

void CConsoleClient::ProcessQuote( CTP::Quote* pQuote )
{
	m_streamOut.str("");
	m_streamOut << pQuote->symbol() << ", ";
	m_streamOut << pQuote->last() << ", ";
	m_streamOut << pQuote->update_time() << endl;

	logger.Info(m_streamOut.str());
}

void CConsoleClient::OnRegisterResult( bool succ, std::string& errMsg )
{
	ClientBase::OnRegisterResult(succ, errMsg);

	// forward the response to real remote client if existing, but don't need to forward login result
	//if(m_pRemoteClient != NULL)
	//	m_pRemoteClient->OnRegisterResult(succ, errMsg);

	m_streamOut.str("");
	m_streamOut << "Logged in to trading front: " << succ ? "Succeeded" : "Failed";
	m_streamOut << " (" << errMsg << ")" << endl;

	logger.Info(m_streamOut.str());
}

bool CConsoleClient::Login( const std::string& brokerId, const std::string& userId, const std::string& password )
{
	ClientBase* pRemoteClient = g_orderMgr.GetCurrentClient();
	if(pRemoteClient != this)
	{
		m_pRemoteClient = pRemoteClient;
		return ClientBase::Login(brokerId, userId, password);
	}
	else
	{
		return false;
	}
}

void CConsoleClient::Logout()
{
	ClientBase* pRemoteClient = g_orderMgr.GetCurrentClient();
	if(pRemoteClient != this)
	{
		cout << "Didn't I log in?" << endl;
	}
	else if(m_pRemoteClient == NULL)
	{
		cout << "I did REALLY log in trading front, now I wanna log out" << endl;
		ClientBase::Logout();
	}
	else 
	{
		cout << "I did just JOIN remote client, now I wanna UNJOIN" << endl;
		g_orderMgr.SetCurrentClient(m_pRemoteClient);
		m_pRemoteClient = NULL;
		m_tradeLoggedin = false;
	}
}
