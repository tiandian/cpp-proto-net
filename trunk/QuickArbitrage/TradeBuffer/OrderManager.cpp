#include "StdAfx.h"
#include "OrderManager.h"
#include "QuoteAggregator.h"
#include "LogManager.h"

#include "boost/foreach.hpp"

using namespace std;

extern CQuoteAggregator g_quoteAggregator;
extern CLogManager	logger;


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
	bool ret = true;

	CPortfolio* pPortfolio = GetPortfolio(pid);
	if(pPortfolio != NULL)
	{
		const LegVector& legs = pPortfolio->GetLegs();

		BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, legs)
		{
			// create input order
			boost::shared_ptr<protoc::InputOrder> order = CreateInputOrderByLeg(leg.get());
			// record the OrderRef
			leg->SetOrderRef(order->orderref());
			leg->SetStatus(IS_OPENING);
			// The status will be set when order is accepted by exchange

			m_orderRepo.AddOrderItem(leg.get());

			bool submit = m_tradeAgent.SubmitOrder(order.get());
			if(!submit) ret = false;
		}
	}

	return ret;
}

bool COrderManager::Portfolio_ClosePosition( const boost::uuids::uuid& pid )
{
	bool ret = true;

	CPortfolio* pPortfolio = GetPortfolio(pid);
	if(pPortfolio != NULL)
	{
		const LegVector& legs = pPortfolio->GetLegs();

		BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, legs)
		{
			// create input order
			boost::shared_ptr<protoc::InputOrder> order = CreateInputOrderByLeg(leg.get());
			// record the OrderRef
			leg->SetStatus(IS_CLOSING);
			
			bool submit = m_tradeAgent.SubmitOrder(order.get());
			if(!submit) ret = false;
		}
	}

	return ret;
}

const char* COrderManager::NextOrderRef()
{
	boost::lock_guard<boost::mutex> lock(m_mutOrderRef);
	++m_orderRefID;
	sprintf(ORDER_REF_BUF, "%d", m_orderRefID);
	return ORDER_REF_BUF;
}

void COrderManager::OnRspOrderInsert( bool succ, const std::string& orderRef, const std::string& msg)
{
	if(orderRef.length() == 0)
	{
		logger.Warning("returned OrderRef is empty while OnRspOrderInsert");
		return;
	}

	// Once get here, there must be something wrong while order insert action
	if(!succ)
	{
		COrderItem* pOrderItem = m_orderRepo.GetOrderItem(orderRef);
		(pOrderItem->GetLeg())->SetMessage(msg);
	}
}


void COrderManager::OnRtnOrder( protoc::Order* order )
{
	string orderRef = order->orderref();
	
	if(order != NULL)
	{
		COrderItem* pOrderItem = m_orderRepo.GetOrderItem(orderRef);
		// Leg status will be changed accordingly HERE.
		pOrderItem->SetOrder(OrderPtr(order));
	}
	else
	{
		logger.Warning("OrderInsert didn't return order object!");
	}

}

const char* TradeDirectionToText(protoc::TradeDirectionType direction)
{
	static char directionText[][5] = { "BUY", "SELL" };
	return directionText[direction - protoc::TradeDirectionType::BUY];
}

const char* OffsetFlagToText(protoc::OffsetFlagType offsetFlag)
{
	static char offsetText[][18] = {
		"OPEN",
		"CLOSE",
		"FORCE_CLOSE",
		"CLOSE_TODAY",
		"CLOSE_YESTERDAY",
		"FORCE_OFF",
		"LOCAL_FORCE_CLOSE"
	};
	return offsetText[offsetFlag - protoc::OF_OPEN];
}

void COrderManager::OnRtnTrade( protoc::Trade* pTrade )
{
	ostringstream oss;
	oss << setw(10) << pTrade->instrumentid() << ": " << setw(10) << TradeDirectionToText(pTrade->direction());
	oss << setw(10) << OffsetFlagToText(pTrade->offsetflag()) << setw(10) << pTrade->price() << ", " << pTrade->volume() << ", " << pTrade->tradetime();
	logger.Info(oss.str());

	
	const string& orderRef = pTrade->orderref();
	COrderItem* pOrderItem = m_orderRepo.GetOrderItem(orderRef);

	if(pOrderItem != NULL)
	{
		// update leg info
		pOrderItem->UpdateTrade(pTrade);
	}
	else
	{
		logger.Warning("Returned trade has no corresponding order!");
	}
}

void COrderManager::OnRspUserLogin( bool succ, std::string& msg, int initOrderRefID )
{
	if(m_pClient != NULL)
	{
		m_pClient->OnRegisterResult(succ, msg);
	}

	if(succ){
		m_orderRefID = initOrderRefID;
	}
	else{
		SetCurrentClient(NULL);
	}
}

boost::shared_ptr<protoc::InputOrder> COrderManager::CreateInputOrderByLeg( CLeg* leg )
{
	LEG_STATUS status = leg->GetStatus();
	assert(status == UNOPENED || status == OPENED);

	boost::shared_ptr<protoc::InputOrder> order(new protoc::InputOrder);
	//order->set_brokerid("0240");
	//order->set_investorid("0240050002");
	order->set_instrumentid(leg->GetSymbol());
	order->set_orderref(NextOrderRef());

	protoc::PosiDirectionType side = leg->GetSide();
	static char CombOffset[1];
	if(status == UNOPENED)
	{
		// in case wanna open position
		if(side == protoc::LONG)
		{
			// open long position
			order->set_direction(protoc::BUY);
		}
		else if(side == protoc::SHORT)
		{
			order->set_direction(protoc::SELL);
		}
		else
		{
			throw std::exception("unexpected leg side");
		}
		order->set_orderpricetype(leg->GetOpenOrderPriceType());
		order->set_limitprice(leg->GetOpenLimitPrice());
		CombOffset[0] = protoc::OF_OPEN;
	}
	else if(status == OPENED)
	{
		// in case wanna close position
		if(side == protoc::LONG)
		{
			order->set_direction(protoc::SELL);
		}
		else if(side == protoc::SHORT)
		{
			order->set_direction(protoc::BUY);
		}
		else
		{
			throw std::exception("unexpected leg side");
		}
		order->set_orderpricetype(leg->GetOpenOrderPriceType());
		order->set_limitprice(leg->GetOpenLimitPrice());
		CombOffset[0] = protoc::OF_CLOSE;
	}
	else
	{
		// unexpected to be here, return NULL
		return boost::shared_ptr<protoc::InputOrder>();
	}
	
	//order->set_orderpricetype(protoc::LIMIT_PRICE);

	//order->set_direction(leg->GetSide() == protoc::LONG ? protoc::BUY : protoc::SELL);

// 	char CombOffset[] = { static_cast<char>(protoc::OF_OPEN) };
 	order->set_comboffsetflag(std::string(CombOffset));

	static char CombHedgeFlag[] = { static_cast<char>(protoc::SPECULATION) };
	order->set_combhedgeflag(std::string(CombHedgeFlag));

// 	order->set_limitprice(0);
	order->set_volumetotaloriginal(leg->GetQuantity());
	order->set_timecondition(protoc::TC_GFD);

	order->set_volumecondition(protoc::VC_AV);
	order->set_minvolume(1);

	order->set_contingentcondition(protoc::IMMEDIATELY);
	order->set_forceclosereason(protoc::NOT_FORCE_CLOSE);
	order->set_isautosuspend(false);
	order->set_userforceclose(false);

	return order;
}
