#include "StdAfx.h"
#include "OrderManager.h"
#include "QuoteAggregator.h"

using namespace std;

extern CQuoteAggregator g_quoteAggregator;


COrderManager::COrderManager(void):
	m_pClient(NULL),
	m_tradeAgent(this)
{
}


COrderManager::~COrderManager(void)
{
}

void COrderManager::OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote)
{

}

void COrderManager::Initialize()
{
	
}

void COrderManager::Subscribe( vector<string>& symbols )
{
	if(GetUuid().is_nil())
	{
		SetSymbols(symbols);
		g_quoteAggregator.SubscribeQuotes(this);
	}
	else
	{
		if(symbols.size() > 0)
			g_quoteAggregator.ChangeQuotes(this, symbols);
		else
			UnSubscribe();
	}
}

void COrderManager::UnSubscribe()
{
	if(!(GetUuid().is_nil()))
		g_quoteAggregator.UnsubscribeQuotes(GetUuid());
}

bool COrderManager::Register( ClientBase* pClient, const std::string& brokerId, const std::string& userId, const std::string& password )
{
	if(m_pClient == NULL)
	{
		SetCurrentClient(pClient);
		m_tradeAgent.Login(brokerId, userId, password);
	}
	else
	{
		boost::tuple<std::string&, std::string&, std::string&> ret = m_tradeAgent.GetCurrentUserInfo();
		if(brokerId == boost::get<0>(ret) && userId == boost::get<1>(ret) && password == boost::get<2>(ret))
		{
			SetCurrentClient(pClient);
			OnRspUserLogin(true, std::string(""));
		}
		else
		{
			return false;
		}
	}

	
	return true;
}

void COrderManager::Unregister( std::string& brokerId, std::string& userId )
{
	boost::tuple<std::string&, std::string&, std::string&> ret = m_tradeAgent.GetCurrentUserInfo();
	if(brokerId == boost::get<0>(ret) && userId == boost::get<1>(ret))
	{
		m_tradeAgent.Logout();
		m_pClient = NULL;
	}
}
