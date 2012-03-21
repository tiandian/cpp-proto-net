#include "StdAfx.h"
#include "OrderManager.h"
#include "QuoteAggregator.h"
#include "OrderHelper.h"


using namespace std;

extern CQuoteAggregator g_quoteAggregator;


COrderManager::COrderManager(void):
	m_pClient(NULL),
	m_orderRefID(-1),
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
			OnRspUserLogin(true, std::string(""), m_orderRefID); // order ref id actually doesn't change 
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

void COrderManager::RemovePortfolio( const boost::uuids::uuid& pid )
{
	PortfolioVecIter foundIter = FindPortfolio(pid);
	if(foundIter != m_portfolioVec.end())
	{
		m_portfolioVec.erase(foundIter);
	}
}

void COrderManager::AddPortfolio( CPortfolio* pPortfolio )
{
	boost::shared_ptr<CPortfolio> portfolio(pPortfolio);
	m_portfolioVec.push_back(portfolio);
}

PortfolioVecIter COrderManager::FindPortfolio( const boost::uuids::uuid& pid )
{
	PortfolioVecIter foundIter = m_portfolioVec.end();
	for (PortfolioVecIter iter = m_portfolioVec.begin();iter != m_portfolioVec.end(); ++iter)
	{
		if((*iter)->GetID() == pid)
		{
			foundIter = iter;
			break;
		}
	}
	return foundIter;
}

CPortfolio* COrderManager::GetPortfolio( const boost::uuids::uuid& pid )
{
	PortfolioVecIter foundIter = FindPortfolio(pid);
	if(foundIter != m_portfolioVec.end())
	{
		return (*foundIter).get();
	}
	else
	{
		return NULL;
	}
}

bool COrderManager::Portfolio_OpenPosition( const boost::uuids::uuid& pid )
{
	bool ret = false;

	CPortfolio* pPortfolio = GetPortfolio(pid);
	if(pPortfolio != NULL)
	{
		boost::shared_ptr<InputOrderVec> orders = CreateOpenPositionOrders(pPortfolio, boost::bind(&COrderManager::NextOrderRef, this));
		
	}

	return ret;
}

bool COrderManager::Portfolio_ClosePosition( const boost::uuids::uuid& pid )
{
	bool ret = false;

	return ret;
}

const char* COrderManager::NextOrderRef()
{
	boost::lock_guard<boost::mutex> lock(m_mutOrderRef);
	++m_orderRefID;
	sprintf(ORDER_REF_BUF, "%d", m_orderRefID);
	return ORDER_REF_BUF;
}

void COrderManager::OnRspOrderInsert( bool succ, std::string& msg, protoc::Order* order )
{

}

void COrderManager::OnRtnTrade( protoc::Trade* pTrade )
{

}
