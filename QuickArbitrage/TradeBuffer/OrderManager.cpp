#include "StdAfx.h"
#include "OrderManager.h"
#include "QuoteAggregator.h"
#include "LogManager.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>


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
	ReadLock lock(m_lock);
	const string& symb = pQuote->symbol();
	pair<SymbolPortfolioMapIter, SymbolPortfolioMapIter> ret = m_dispatchMap.equal_range(symb);
	for (SymbolPortfolioMapIter it = ret.first; it != ret.second; ++it)
	{
		(it->second)->UpdateQuote(pQuote.get());
	}
}

void COrderManager::AddPortfolioListenQuote(CPortfolio* portfolio, bool submit/* = true*/)
{
	WriteLock lock(m_lock);
	BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, portfolio->GetLegs())
	{
		const string& symb = leg->GetSymbol();
		m_dispatchMap.insert(pair<string, CPortfolio*>(symb, portfolio));
	}
	if(submit)
		UpdateListeningQuote();
}

void COrderManager::RemovePortfolioListenQuote(CPortfolio* portfolio)
{
	WriteLock lock(m_lock);
	BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, portfolio->GetLegs())
	{
		const string& symb = leg->GetSymbol();
		pair<SymbolPortfolioMapIter, SymbolPortfolioMapIter> ret = m_dispatchMap.equal_range(symb);
		for (SymbolPortfolioMapIter it = ret.first; it != ret.second; ++it)
		{
			if(it->second == portfolio)
			{
				// find it;
				m_dispatchMap.erase(it);
				break;
			}
		}
	}
	UpdateListeningQuote();
}

void COrderManager::UpdateListeningQuote()
{
	vector<string> currentSymbols;
	for (SymbolPortfolioMapIter it = m_dispatchMap.begin(); it != m_dispatchMap.end(); ++it)
	{
		currentSymbols.push_back(it->first);
	}
	if(currentSymbols.size() > 0)
		Subscribe(currentSymbols);
	else
		UnSubscribe();
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

		{
			WriteLock lock(m_lock);
			m_dispatchMap.clear();
			// todo:
		}
		m_portfolioVec.clear();
		m_orderRepo.Clear();
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
	AddPortfolio(portfolio);
}

void COrderManager::AddPortfolio(const boost::shared_ptr<CPortfolio>& portfolio, bool submit/* = true*/)
{
	m_portfolioVec.push_back(portfolio);
	if(submit)
		m_database.AddPortfolio(portfolio.get());
	AddPortfolioListenQuote(portfolio.get(), submit);

	// Set Entry trigger here, just for test purpose
	portfolio->GetEntryTrigger()->SetHandler(boost::bind(&COrderManager::Portfolio_OpenPosition, this, _1));
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

CPortfolio* COrderManager::GetPortfolio( int portIdx )
{
	if( portIdx < m_portfolioVec.size() && portIdx >= 0)
	{
		return m_portfolioVec[portIdx].get();
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
			// create input order only when leg is opened
			if(leg->GetStatus() == OPENED)
			{
				boost::shared_ptr<protoc::InputOrder> order = CreateInputOrderByLeg(leg.get());

				// record the OrderRef
				leg->SetOrderRef(order->orderref());
				// record the OrderRef
				leg->SetStatus(IS_CLOSING);

				m_orderRepo.AddOrderItem(leg.get());

				bool submit = m_tradeAgent.SubmitOrder(order.get());
				if(!submit) ret = false;
			}
		}
	}

	return ret;
}

bool COrderManager::Portfolio_CancelLegOrder( const boost::uuids::uuid& pid, int legIdx /*= -1*/ )
{
	bool ret = true;

	CPortfolio* pPortfolio = GetPortfolio(pid);
	if(pPortfolio != NULL)
	{
		const LegVector& legs = pPortfolio->GetLegs();

		int i = 0;
		BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, legs)
		{
			if(legIdx < 0 || i == legIdx)
			{
				// create input order
				boost::shared_ptr<protoc::InputOrderAction> order = CreateCancelActionByLeg(leg.get());

				if(order == NULL)
					continue;	// leg is invalid to create input order, ignore it

				bool submit = m_tradeAgent.SubmitOrderAction(order.get());
				if(!submit) ret = false;
			}
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
		if(pOrderItem != NULL)
		{
			// Leg status will be changed accordingly HERE.
			pOrderItem->SetOrder(OrderPtr(order));
		}
		else
		{
			logger.Warning(boost::str(boost::format("Unexpected order with ref:%s") % orderRef));
		}
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

	m_database.AddTrade(pTrade);
}

void COrderManager::OnRspUserLogin( bool succ, std::string& msg, int initOrderRefID )
{
	if(m_pClient != NULL)
	{
		m_pClient->OnRegisterResult(succ, msg);
	}

	if(succ){
		m_orderRefID = initOrderRefID;
		m_database.EnsureValid(m_tradeAgent.GetUserId().c_str());

		PortfolioVector tmpVec;
		m_database.FetchPortfolio(tmpVec);
		BOOST_FOREACH(const boost::shared_ptr<CPortfolio>& port, tmpVec)
		{
			AddPortfolio(port, false);
		}
		// consolidate submit
		UpdateListeningQuote();
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
		order->set_orderpricetype(leg->GetCloseOrderPriceType());
		order->set_limitprice(leg->GetCloseLimitPrice());
		CombOffset[0] = protoc::OF_CLOSE_TODAY;
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
	
	if(order->orderpricetype() == protoc::ANY_PRICE)
	{
		// IOC needed for market price
		order->set_timecondition(protoc::TC_IOC);
	}
	else
	{
		order->set_timecondition(protoc::TC_GFD);
	}

	order->set_volumecondition(protoc::VC_AV);
	order->set_minvolume(1);

	order->set_contingentcondition(protoc::IMMEDIATELY);
	order->set_forceclosereason(protoc::NOT_FORCE_CLOSE);
	order->set_isautosuspend(false);
	order->set_userforceclose(false);

	return order;
}

boost::shared_ptr<protoc::InputOrderAction> COrderManager::CreateCancelActionByLeg( CLeg* leg )
{
	LEG_STATUS status = leg->GetStatus();
	if(status == IS_OPENING || status == IS_CLOSING)
	{
		boost::shared_ptr<protoc::InputOrderAction> orderAction(new protoc::InputOrderAction);
		
		std::string orderRefToCancel = leg->GetOrderRef();
		orderAction->set_orderref(orderRefToCancel);

		///操作标志
		orderAction->set_actionflag(protoc::AF_Delete);	// Cancel order

		COrderItem* pOrderItem = m_orderRepo.GetOrderItem(orderRefToCancel);
		if(pOrderItem != NULL)
		{
			///交易所代码
			orderAction->set_exchangeid(pOrderItem->GetOrder()->exchangeid());
			///报单编号
			orderAction->set_ordersysid(pOrderItem->GetOrder()->ordersysid());
			///用户代码
			orderAction->set_userid(pOrderItem->GetOrder()->userid());
		}

		orderAction->set_instrumentid(leg->GetSymbol());
		
		return orderAction;
	}
	else	// return null pointer
		return boost::shared_ptr<protoc::InputOrderAction>();
}

void COrderManager::OnRspOrderAction( bool succ, const std::string& orderRef, const std::string& msg )
{
	if(orderRef.length() == 0)
	{
		logger.Warning("returned OrderRef is empty while OnRspOrderAction");
		return;
	}

	// Once get here, there must be something wrong while order insert action
	if(succ)
	{
		logger.Info(boost::str(boost::format("Cancel order(orderRef:%s) succeeded.") % orderRef));
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cancel order(orderRef:%s) failed due to %s") % orderRef % msg));
	}
}
