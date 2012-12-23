#include "StdAfx.h"
#include "ClientAgent.h"
#include "Portfolio.h"
#include "DiffStrategy.h"
#include "PortfolioOrderHelper.h"

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

CClientAgent::CClientAgent(void):
m_clientConnected(false)
{
	m_quoteAggregator.Initialize(&m_quoteAgent);
	
	m_portfolioMgr.SetQuoteAggregator(&m_quoteAggregator);
	m_portfolioMgr.SetPushPortfolioFunc(boost::bind(&CClientAgent::OnPortfolioUpdated, this, _1));
	
	m_orderProcessor.Initialize(&m_tradeAgent);
	m_orderProcessor.SetPushPortfolioFunc(boost::bind(&CClientAgent::OnMultiLegOrderUpdated, this, _1));
	m_orderProcessor.SetPushTradeFunc(boost::bind(&CClientAgent::OnTradeUpdated, this, _1));
	m_orderProcessor.SetPushPositionDetailFunc(boost::bind(&CClientAgent::OnPostionDetailReturned, this, _1));
	m_orderProcessor.SetPushOrderFunc(boost::bind(&CClientAgent::OnLegOrderUpdated, this, _1, _2, _3));

	m_orderProcessor.SetPushPositionChangeFunc(boost::bind(&CPortfolioManager::PortfolioPositionChange, &m_portfolioMgr, _1));
	m_orderProcessor.SetPushResubmitterChangeFunc(boost::bind(&CPortfolioManager::ChangePortfolioResubmitter, &m_portfolioMgr, _1, _2, _3));
	m_orderProcessor.SetPortfolioPlaceOrderDoneFunc(boost::bind(&CPortfolioManager::PortfolioPlaceOrderDone, &m_portfolioMgr, _1));
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

void CClientAgent::AddPortfolios( entity::AddPortfolioParam& addPortfParam )
{
	m_quoteAggregator.DelaySubmit();

	int count = addPortfParam.portfolioitems_size();
	for(int i = 0; i < count; ++i )
	{
		const entity::PortfolioItem& portfItem = addPortfParam.portfolioitems(i);
		entity::PortfolioItem* pClonedItem = new entity::PortfolioItem;
		pClonedItem->CopyFrom(portfItem);
		Add(pClonedItem);
	}

	m_quoteAggregator.SubmitSubscription();
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

void CClientAgent::OpenPosition( const string& pid, int quantity)
{
	// build order
	CPortfolio* portf = m_portfolioMgr.Get(pid);
	OpenPosition(portf, quantity, trade::SR_Manual);
}


void CClientAgent::OpenPosition( CPortfolio* portf, int qty, trade::SubmitReason submitReason)
{
	bool autoTracking = portf->Strategy()->IsAutoTracking();
	bool enablePrefer = portf->Strategy()->EnablePrefer();

	PlaceOrderContext placeOrderCtx;
	placeOrderCtx.quantity = qty;
	placeOrderCtx.brokerId = m_brokerId;
	placeOrderCtx.investorId = m_userId;
	placeOrderCtx.orderPriceType = trade::LIMIT_PRICE;
	placeOrderCtx.limitPriceType = entity::Opposite;

	boost::shared_ptr<trade::MultiLegOrder> multilegOrder(BuildOpenPosiOrder(portf, &placeOrderCtx));
	multilegOrder->set_reason(submitReason);

	portf->BeginPlaceOrder();
	// send to order processor
	if(enablePrefer)
		m_orderProcessor.SubmitOrder2(multilegOrder, autoTracking);
	else
		m_orderProcessor.SubmitOrder(multilegOrder);
}

void CClientAgent::ClosePosition( const trade::MultiLegOrder& openMlOrd, const string& legOrdRef, trade::SubmitReason submitReason, string& msg)
{
	CPortfolio* portf = m_portfolioMgr.Get(openMlOrd.portfolioid());

	bool autoTracking = portf->Strategy()->IsAutoTracking();
	bool enablePrefer = portf->Strategy()->EnablePrefer();

	PlaceOrderContext placeOrderCtx;
	placeOrderCtx.brokerId = m_brokerId;
	placeOrderCtx.investorId = m_userId;
	placeOrderCtx.orderPriceType = trade::LIMIT_PRICE;
	placeOrderCtx.limitPriceType = entity::Opposite;

	boost::shared_ptr<trade::MultiLegOrder> multilegOrder(BuildClosePosiOrder(portf,
		&openMlOrd, openMlOrd.quantity(), &placeOrderCtx));
	multilegOrder->set_reason(submitReason);

	portf->BeginPlaceOrder();

	if(enablePrefer)
		m_orderProcessor.SubmitOrder2(multilegOrder, autoTracking);
	else
		m_orderProcessor.SubmitOrder(multilegOrder);
}

void CClientAgent::SimpleCloseOrderPosition(const string& portfolioId, trade::SubmitReason submitReason)
{
	CPortfolio* portf = m_portfolioMgr.Get(portfolioId);

	std::vector<MultiLegOrderPtr> openedOrders;
	int orderCount = portf->GetPosition(openedOrders);
	std::vector<MultiLegOrderPtr>::iterator iter = openedOrders.begin();
	if(iter != openedOrders.end())
	{
		const trade::MultiLegOrder& openMlOrd = *(*iter);
		std::string msg;
		ClosePosition(openMlOrd, std::string(), submitReason, msg);
	}
}

void CClientAgent::ClosePosition(const string& portfolioId, int quantity, trade::SubmitReason submitReason)
{
	CPortfolio* portf = m_portfolioMgr.Get(portfolioId);

	bool autoTracking = portf->Strategy()->IsAutoTracking();
	bool enablePrefer = portf->Strategy()->EnablePrefer();

	PlaceOrderContext placeOrderCtx;
	placeOrderCtx.brokerId = m_brokerId;
	placeOrderCtx.investorId = m_userId;
	placeOrderCtx.orderPriceType = trade::LIMIT_PRICE;
	placeOrderCtx.limitPriceType = entity::Opposite;

	boost::shared_ptr<trade::MultiLegOrder> multilegOrder(BuildClosePosiOrder(portf,
		NULL, quantity, &placeOrderCtx));
	multilegOrder->set_reason(submitReason);

	portf->BeginPlaceOrder();

	if(enablePrefer)
		m_orderProcessor.SubmitOrder2(multilegOrder, autoTracking);
	else
		m_orderProcessor.SubmitOrder(multilegOrder);
}

void CClientAgent::ChangePosition(CPortfolio* portf, const string& closeSymbol, entity::PosiDirectionType existingPosition, int qty, trade::SubmitReason submitReason)
{
	PlaceOrderContext placeOrderCtx;
	placeOrderCtx.quantity = qty;
	placeOrderCtx.brokerId = m_brokerId;
	placeOrderCtx.investorId = m_userId;
	placeOrderCtx.orderPriceType = trade::LIMIT_PRICE;
	placeOrderCtx.limitPriceType = entity::Opposite;

	boost::shared_ptr<trade::MultiLegOrder> multilegOrder(BuildChangePosiOrder(portf,
		closeSymbol, existingPosition, &placeOrderCtx));
	bool autoTracking = portf->Strategy()->IsAutoTracking();
	multilegOrder->set_reason(submitReason);

	portf->BeginPlaceOrder();

	m_orderProcessor.SubmitOrder2(multilegOrder, autoTracking);
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

void CClientAgent::EnableStrategy(const entity::ModifyRunningStatusParam& runningStatusParam)
{
	const string& portfId = runningStatusParam.portfid();
	CPortfolio* portf = m_portfolioMgr.Get(portfId);
	if(portf != NULL)
	{
		bool isEnabled = runningStatusParam.enabled();
		portf->EnableStrategy(isEnabled);
	}
}

void CClientAgent::TurnPortfSwitches( const entity::ModifyPortfolioSwitchParam& switchesParam )
{
	const string& portfId = switchesParam.portfid();
	CPortfolio* portf = m_portfolioMgr.Get(portfId);
	if(portf != NULL)
	{
		portf->TurnSwitches(switchesParam.autoopen(),
			switchesParam.autostopgain(), switchesParam.autostoploss(),
			switchesParam.autotracking(), switchesParam.enableprefer());
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

void CClientAgent::CancelOrder( const entity::CancelOrderParam& cancelParam )
{
	m_orderProcessor.CancelOrder(cancelParam.orderref(), cancelParam.exchangeid(),
		cancelParam.ordsysid(), m_userId, cancelParam.symbol());
}

void CClientAgent::OnPostionDetailReturned( trade::PositionDetailInfo* posiDetailInfo )
{
	std::string callbackData;
	posiDetailInfo->SerializeToString(&callbackData);
	if(m_pSession != NULL)
		m_pSession->BeginCallback("PositionDetailReturn", callbackData);
}

void CClientAgent::QueryPositionDetails( const string& symbol )
{
	m_orderProcessor.QueryPositionDetails(symbol);
}

boost::tuple<bool, string> CClientAgent::ManualCloseOrder( const string& symbol, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, int quantity )
{
	PlaceOrderContext placeOrderCtx;
	placeOrderCtx.quantity = quantity;
	placeOrderCtx.brokerId = m_brokerId;
	placeOrderCtx.investorId = m_userId;
	placeOrderCtx.orderPriceType = trade::LIMIT_PRICE;
	placeOrderCtx.limitPriceType = entity::Opposite;

	//return m_orderProcessor.ManualCloseOrder(symbol, direction, offsetFlag, &placeOrderCtx);
	return m_orderProcessor.PlaceOrder(symbol, direction, offsetFlag, &placeOrderCtx);
}

void CClientAgent::VirtualOpenPosition( const string& pid, int quantity )
{
	CPortfolio* portf = m_portfolioMgr.Get(pid);
	if(portf != NULL)
		portf->VirtualOpen(quantity);
}

void CClientAgent::VirtualClosePosition( const string& pid, int quantity )
{
	CPortfolio* portf = m_portfolioMgr.Get(pid);
	if(portf != NULL)
		portf->VirtualClose(quantity);
}

void CClientAgent::SetPortfolioQuantity( const string& pid, int qty, int maxQty )
{
	CPortfolio* portf = m_portfolioMgr.Get(pid);
	if(portf != NULL)
		portf->SetQuantity(qty, maxQty);
}


