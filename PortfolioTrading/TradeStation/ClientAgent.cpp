#include "StdAfx.h"
#include "ClientAgent.h"
#include "Portfolio.h"
#include "PortfolioOrderHelper.h"

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

CClientAgent::CClientAgent(void):
m_clientConnected(false)
{
	m_quoteAggregator.Initialize(&m_quoteAgent);
	
	m_portfolioMgr.SetQuoteAggregator(&m_quoteAggregator);
	m_portfolioMgr.SetPushPortfolioFunc(boost::bind(&CClientAgent::OnPortfolioUpdated, this, _1));
	m_portfolioMgr.SetOpenPortfolioFunc(boost::bind(&CClientAgent::OpenPosition, this, _1, _2));
	
	m_orderProcessor.Initialize(&m_tradeAgent);
	m_orderProcessor.SetPushPortfolioFunc(boost::bind(&CClientAgent::OnMultiLegOrderUpdated, this, _1));
	m_orderProcessor.SetPushTradeFunc(boost::bind(&CClientAgent::OnTradeUpdated, this, _1));
	m_orderProcessor.SetPushOrderFunc(boost::bind(&CClientAgent::OnLegOrderUpdated, this, _1, _2, _3));
	m_orderProcessor.SetPushPositionChangeFunc(boost::bind(&CPortfolioManager::PortfolioPositionChange, &m_portfolioMgr, _1));
}

CClientAgent::~CClientAgent(void)
{
}

void CClientAgent::Add( entity::PortfolioItem* portfolioItem )
{
	// let CPortfolio wrap and hold entity::PortfolioItem instance
	CPortfolio* pPortf = CPortfolio::Create(this, portfolioItem); 
	m_portfolioMgr.Add(pPortf);
}

void CClientAgent::Remove( const string& pid )
{
	m_portfolioMgr.Remove(pid);
}

boost::tuple<bool, string> CClientAgent::TradeConnect( const std::string& address, const std::string& streamDir )
{
	m_tradeAddress = address;
	return m_tradeAgent.Open(m_tradeAddress, streamDir);
}

boost::tuple<bool, string> CClientAgent::TradeLogin( const string& brokerId, const string& userId, const string& password )
{
	m_brokerId = brokerId;
	m_userId = userId;
	return m_tradeAgent.Login(brokerId, userId, password);
}

void CClientAgent::TradeLogout()
{
	m_tradeAgent.Logout();
}

void CClientAgent::TradeDisconnect()
{
	m_tradeAgent.Logout();
	m_tradeAgent.Close();
}

boost::tuple<bool, string> CClientAgent::QuoteLogin( const string& brokerId, const string& userId, const string& password )
{
	return m_quoteAgent.Login(brokerId, userId, password);
}

void CClientAgent::QuoteLogout()
{
	m_quoteAgent.Logout();
}

boost::tuple<bool, string> CClientAgent::QuoteConnect( const std::string& address, const std::string& streamDir )
{
	m_quoteAddress = address;
	return m_quoteAgent.Open(m_quoteAddress, streamDir);
}

void CClientAgent::QuoteDisconnect()
{
	m_quoteAgent.Logout();
	m_quoteAgent.Close();
}

void CClientAgent::RegQuote( vector<string>& symbols )
{
	if(symbols.size() > 0)
	{
		if(!Registered())
		{
			SetSymbols(symbols);
			m_quoteAggregator.SubscribeQuotes(this);
		}
		else
		{
			m_quoteAggregator.ChangeQuotes(this, symbols);
		}
	}
	else
	{
		m_quoteAggregator.UnsubscribeQuotes(this);
	}
}

void CClientAgent::OnQuoteRecevied( boost::shared_ptr<entity::Quote>& pQuote )
{
	std::string callbackData;
	pQuote->SerializeToString(&callbackData);
	m_pSession->BeginCallback("QuotePush", callbackData);
}

void CClientAgent::PortfolioOpenPosition( const string& pid, int quantity )
{
	// build order
	CPortfolio* portf = m_portfolioMgr.Get(pid);
	OpenPosition(portf, quantity);
}


void CClientAgent::OpenPosition( CPortfolio* portf, int qty )
{
	PlaceOrderContext placeOrderCtx;
	placeOrderCtx.quantity = qty;
	placeOrderCtx.brokerId = m_brokerId;
	placeOrderCtx.investorId = m_userId;
	placeOrderCtx.orderPriceType = trade::LIMIT_PRICE;
	placeOrderCtx.limitPriceType = entity::Opposite;

	boost::shared_ptr<trade::MultiLegOrder> multilegOrder(BuildOpenPosiOrder(portf, &placeOrderCtx));
	// send to order processor
	m_orderProcessor.SubmitOrder(multilegOrder);
}

void CClientAgent::ClosePosition( const trade::MultiLegOrder& openMlOrd, const string& legOrdRef, string& msg)
{
	CPortfolio* portf = m_portfolioMgr.Get(openMlOrd.portfolioid());
	PlaceOrderContext placeOrderCtx;
	placeOrderCtx.brokerId = m_brokerId;
	placeOrderCtx.investorId = m_userId;
	placeOrderCtx.orderPriceType = trade::LIMIT_PRICE;
	placeOrderCtx.limitPriceType = entity::Opposite;

	boost::shared_ptr<trade::MultiLegOrder> multilegOrder(BuildClosePosiOrder(portf,
		&openMlOrd, &placeOrderCtx));
	m_orderProcessor.SubmitOrder(multilegOrder);
}

bool CClientAgent::QueryAccountInfo(string* serializedAcctInfo)
{
	bool succ = m_orderProcessor.QueryAccountInfo(serializedAcctInfo);
	return succ;
}

void CClientAgent::OnPortfolioUpdated(entity::PortfolioItem* portfolioItem)
{
	std::string callbackData;
	portfolioItem->SerializeToString(&callbackData);
	if(m_pSession != NULL)
		m_pSession->BeginCallback("PortfolioPush", callbackData);
}

void CClientAgent::OnMultiLegOrderUpdated( trade::MultiLegOrder* order )
{
	std::string callbackData;
	order->SerializeToString(&callbackData);
	if(m_pSession != NULL)
		m_pSession->BeginCallback("MultiLegOrderPush", callbackData);
}

void CClientAgent::OnTradeUpdated( trade::Trade* trade )
{
	std::string callbackData;
	trade->SerializeToString(&callbackData);
	if(m_pSession != NULL)
		m_pSession->BeginCallback("TradePush", callbackData);
}

void CClientAgent::OnLegOrderUpdated( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	entity::LegOrderUpdateParam orderUpdate;
	orderUpdate.set_portfid(portfId);
	orderUpdate.set_multilegorderid(mlOrderId);
	orderUpdate.set_legorderref(legOrd->orderref());
	
	trade::Order* pOrd = orderUpdate.mutable_legorder();
	pOrd->CopyFrom(*legOrd);
	    
	std::string callbackData;
	orderUpdate.SerializeToString(&callbackData);
	if(m_pSession != NULL)
		m_pSession->BeginCallback("LegOrderPush", callbackData);
}

void CClientAgent::ApplyStrategySetting( const entity::ModifyStrategyParam& settings )
{
	const string& portfId = settings.portfid();
	CPortfolio* portf = m_portfolioMgr.Get(portfId);
	if(portf != NULL)
	{
		portf->ApplyStrategySetting(settings.strategyname(), settings.strategydata());
	}
}

void CClientAgent::TurnPortfSwitches( const entity::ModifyPortfolioSwitchParam& switchesParam )
{
	const string& portfId = switchesParam.portfid();
	CPortfolio* portf = m_portfolioMgr.Get(portfId);
	if(portf != NULL)
	{
		bool isEnabled = switchesParam.enabled();
		if(isEnabled)
		{
			portf->TurnSwitches(switchesParam.autoopen(),
				switchesParam.autostopgain(), switchesParam.autostoploss());
		}
		
		portf->EnableStrategy(isEnabled);
	}
}

void CClientAgent::SetPorfPreferredLeg( const entity::ModifyPortfolioPreferredLegParam& preferredLegParam )
{
	const string& portfId = preferredLegParam.portfid();
	CPortfolio* portf = m_portfolioMgr.Get(portfId);
	const string& legName = preferredLegParam.legsymbol();
	if(portf != NULL)
	{
		BOOST_FOREACH(const LegPtr& l, portf->Legs())
		{
			if(l->Symbol() == legName)
				l->IsPreferred(true);
			else
				l->IsPreferred(false);
		}
	}
}
