#include "OrderProcessor2.h"
#include "globalmembers.h"
#include "orderhelper.h"
#include "TradeAgent.h"
#include "ClientAgent.h"
#include "charsetconvert.h"
#include "SymbolInfoRepositry.h"
#include "PortfolioOrderPlacer.h"
#include "InputOrder.h"

#include <boost/format.hpp>
#include <boost/date_time.hpp>

#ifndef WIN32
#define sprintf_s sprintf 
#endif

extern CSymbolInfoRepositry symbolInfoRepo;

const char EX_SHFE[] = "SHFE";
const char EX_DCE[] = "DCE";
const char EX_CZCE[] = "CZCE";
const char EX_FFEX[] = "FFEX";

const char* TRADE_DIRECTION[] = {"Buy", "Sell"};

void PrintInputOrder(CInputOrder* order)
{
	if(order != NULL)
	{
		LOG_INFO(logger, boost::str(boost::format("[%s] - %s %d %s @ %.2f")
			% ((order->OffsetFlag()[0] == trade::OF_OPEN) ? "OPEN" : "CLOSE")
			% TRADE_DIRECTION[order->Direction() - trade::BUY] % order->VolumeTotalOriginal()
			% order->Symbol() % order->LimitPrice()));
	}
}

COrderProcessor2::COrderProcessor2(void):
m_pTradeAgent(NULL),
m_pClientAgent(NULL),
m_maxOrderRef(0),
m_bIsSubmitting(false),
m_totalOpenTimes(0),
m_totalCancelTimes(0),
m_maxTotalOpenTimes(450),
m_maxTotalCancelTimes(900)
{
	memset(&m_orderRefBuf, 0, sizeof(m_orderRefBuf));
}


COrderProcessor2::~COrderProcessor2(void)
{
}

void COrderProcessor2::OnRspUserLogin( bool succ, std::string& msg, int initOrderRefID )
{
	if(succ)
		m_maxOrderRef = initOrderRefID;
	else
	{
		string warn = "Trade login failed due to " + msg;
		logger.Warning(warn);
	}
}

void COrderProcessor2::OnRspOrderInsert( bool succ, const std::string& orderRef, const std::string& msg )
{
	boost::lock_guard<boost::recursive_mutex> lock(m_ordPlacersMapMutex);
	boost::unordered_map<string, CPortfolioOrderPlacer*>::iterator iter 
		= m_workingOrderPlacers.find(orderRef);
	if(iter != m_workingOrderPlacers.end())
	{
		(iter->second)->OnOrderPlaceFailed(msg);
	}
	else
	{
		logger.Warning(boost::str(boost::format("Unexpected order(ref:%s) returned after order insert!") % orderRef));
	}
}

void COrderProcessor2::OnRspOrderAction( bool succ, const std::string& orderRef, int errorId, const std::string& msg )
{
	boost::lock_guard<boost::recursive_mutex> lock(m_ordPlacersMapMutex);
	boost::unordered_map<string, CPortfolioOrderPlacer*>::iterator iter 
		= m_workingOrderPlacers.find(orderRef);
	if(iter != m_workingOrderPlacers.end())
	{
		(iter->second)->OnOrderCancelFailed(errorId, msg);
	}
	else
	{
		logger.Warning(boost::str(boost::format("Unexpected order(ref:%s) returned after order cancel!") % orderRef));
	}
}

void COrderProcessor2::OnRtnOrder( trade::Order* order )
{
	boost::shared_ptr<trade::Order> orderPtr(order);
	DispatchRtnOrder(orderPtr);
}

void COrderProcessor2::OnRtnTrade( trade::Trade* pTrade )
{
	PublishTradeUpdate(pTrade);
}

void COrderProcessor2::OnRspQryInvestorPositionDetail( trade::PositionDetailInfo* pPositionDetail )
{
	PublishPositionDetail(pPositionDetail);
}

void COrderProcessor2::PrintOrderStatus( trade::Order* order )
{
	trade::OrderSubmitStatusType submitStatus = order->ordersubmitstatus();
	trade::OrderStatusType status = order->orderstatus();
	logger.Debug(boost::str(boost::format("Order(%s) - submit status(%s), order status(%s)")
		% order->orderref().c_str() % GetSumbitStatusText(submitStatus) % GetStatusText(status)));
}

void COrderProcessor2::CancelOrder( const std::string& ordRef, const std::string& exchId, const std::string& ordSysId, const std::string& userId, const std::string& symbol )
{
	boost::shared_ptr<trade::InputOrderAction> orderAction(new trade::InputOrderAction);

	orderAction->set_orderref(ordRef);

	///操作标志
	orderAction->set_actionflag(trade::AF_Delete);	// Cancel order

	///交易所代码
	orderAction->set_exchangeid(exchId);
	///报单编号
	orderAction->set_ordersysid(ordSysId);
	///用户代码
	orderAction->set_userid(userId);

	orderAction->set_instrumentid(symbol);

	if(m_pTradeAgent != NULL)
	{
		if(!ReachCancelTimesLimit())
		{
			m_pTradeAgent->SubmitOrderAction(orderAction.get());
			AddCancelTimes();
		}
	}
}

int COrderProcessor2::LockForSubmit( string& outOrdRef )
{
	int retOrderRef = -1;
	boost::unique_lock<boost::mutex> lock(m_mutOrdRefIncr);
	if(m_condSubmit.timed_wait(lock, boost::posix_time::seconds(3), 
		boost::bind(&COrderProcessor2::IsReadyForSubmit, this)))
	{
		retOrderRef = GenerateOrderRef(outOrdRef);
		m_bIsSubmitting = true;
	}
	else
		logger.Warning("LOCK for submit order TIME OUT!");

	return retOrderRef;
}

bool COrderProcessor2::SubmitAndUnlock(CInputOrder* pInputOrder)
{
	boost::mutex::scoped_lock lock(m_mutOrdRefIncr);
#ifdef LOG_FOR_TRADE
	PrintInputOrder(pInputOrder);
#endif
	bool succ = m_pTradeAgent->SubmitOrder(pInputOrder->InnerOrder());
	if(succ)
		AddOpenTimes();
	
	m_bIsSubmitting = false;
	m_condSubmit.notify_one();
	return succ;
}

int COrderProcessor2::GenerateOrderRef( string& outOrdRef )
{
	sprintf_s(m_orderRefBuf, "%d", m_maxOrderRef);
	outOrdRef = m_orderRefBuf;
	int currOrdRef = m_maxOrderRef++;
	return currOrdRef;
}

void COrderProcessor2::PublishMultiLegOrderUpdate( trade::MultiLegOrder* pOrder )
{
	m_pClientAgent->OnMultiLegOrderUpdated(pOrder);
}

void COrderProcessor2::PublishOrderUpdate( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	m_pClientAgent->OnLegOrderUpdated(portfId, mlOrderId, legOrd);
}

void COrderProcessor2::PublishTradeUpdate( trade::Trade* pTrade )
{
	m_pClientAgent->OnTradeUpdated(pTrade);
}

void COrderProcessor2::PublishPositionDetail( trade::PositionDetailInfo* pPosiDetailInfo )
{
	m_pClientAgent->OnPostionDetailReturned(pPosiDetailInfo);
}

void COrderProcessor2::Initialize( CClientAgent* pClientAgent, CTradeAgent* pTradeAgent )
{
	m_pClientAgent = pClientAgent;
	m_pTradeAgent = pTradeAgent;
	if(m_pTradeAgent != NULL)
		m_pTradeAgent->SetCallbackHanlder(this);
}

void COrderProcessor2::OnRspQryTradingAccount( const trade::AccountInfo& accountInfo )
{
	boost::mutex::scoped_lock lock(m_mutQryAcct);
	accountInfo.SerializeToString(&m_serializedQryAcctInfo);
	m_condQryAcct.notify_one();
}

bool COrderProcessor2::QueryAccountInfo( string* outSerializedAcctInfo )
{
	bool ret = false;
	boost::unique_lock<boost::mutex> lock(m_mutQryAcct);
	m_serializedQryAcctInfo.clear();
	m_pTradeAgent->QueryAccount();

	if(m_condQryAcct.timed_wait(lock, boost::posix_time::seconds(15)))
	{
		*outSerializedAcctInfo = m_serializedQryAcctInfo;
		ret = true;
	}

	return ret;
}

void COrderProcessor2::QueryPositionDetails( const string& symbol )
{
	m_pTradeAgent->QueryPositionDetails(symbol);
}

trade::OffsetFlagType GetTradeOffset(const string& exchId, boost::gregorian::date openDate, boost::gregorian::date tradingDay)
{
	if(exchId == EX_SHFE)
	{
		if(openDate < tradingDay)
			return trade::OF_CLOSE;
		else
			return trade::OF_CLOSE_TODAY;
	}
	else
		return trade::OF_CLOSE;
}

trade::InputOrder* COrderProcessor2::BuildCloseOrder( const string& symbol, trade::TradeDirectionType direction, const string& openDate, PlaceOrderContext* placeOrderCtx )
{
	entity::Quote* pQuote = NULL;
	bool succ = m_pTradeAgent->QuerySymbol(symbol, &pQuote);
	if(succ)
	{
		logger.Info(boost::str(boost::format("Query Quote %s: %d") 
			% symbol.c_str() % pQuote->last()));

		double limitPrice = direction == trade::SELL ? pQuote->bid(): pQuote->ask();

		boost::gregorian::date openDt = boost::gregorian::from_undelimited_string(openDate);
		string exchId = symbolInfoRepo.GetExchangeId(symbol);
		trade::OffsetFlagType offsetFlag = GetTradeOffset(exchId, openDt, m_pTradeAgent->TradingDay());

		trade::InputOrder* closeOrder(
			BuildSingleOrder(symbol, limitPrice, direction, offsetFlag, placeOrderCtx));
		
		delete pQuote;

		return closeOrder;
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot query quote %s") % symbol));
	}
	return NULL;
}

boost::tuple<bool, string> COrderProcessor2::PlaceOrder( const string& symbol, trade::TradeDirectionType direction, const string& openDate, PlaceOrderContext* placeOrderCtx )
{
	return boost::make_tuple(false, string("Failed to build close order"));
	/*
	boost::shared_ptr<trade::InputOrder> pInputOrder(BuildCloseOrder(symbol, direction, openDate, placeOrderCtx));
	if(pInputOrder.get() == NULL)
		return boost::make_tuple(false, string("Failed to build close order"));

	CManualSgOrderPlacer* placer = m_sgOrderStateMachine.CreateManualPlacer(pInputOrder, 2, this);
	
	bool succ = placer->DoAndWait();

	string errMsg = placer->GetError();
	
	string pId = placer->Id();
	m_sgOrderStateMachine.RemovePlacer(pId);

	return boost::make_tuple(succ, errMsg);
	*/
}

bool COrderProcessor2::QuerySymbol( const std::string& symbol, entity::Quote** ppQuote )
{
	return m_pTradeAgent->QuerySymbol(symbol, ppQuote);
}

const string& COrderProcessor2::BrokerId()
{
	return m_pTradeAgent->BrokerId();
}

const string& COrderProcessor2::InvestorId()
{
	return m_pTradeAgent->InvestorId();
}

void COrderProcessor2::AddOrderPlacer( CPortfolioOrderPlacer* pOrdPlacer )
{
	boost::lock_guard<boost::recursive_mutex> lock(m_ordPlacersMapMutex);
	m_workingOrderPlacers.insert(std::make_pair(pOrdPlacer->Id(), pOrdPlacer));
}

void COrderProcessor2::RemoveOrderPlacer( const string& placerId /* order ref*/ )
{
	boost::lock_guard<boost::recursive_mutex> lock(m_ordPlacersMapMutex);
	m_workingOrderPlacers.erase(placerId);
}

void COrderProcessor2::DispatchRtnOrder( boost::shared_ptr<trade::Order>& rtnOrder )
{
	boost::lock_guard<boost::recursive_mutex> lock(m_ordPlacersMapMutex);
	const string& orderRef = rtnOrder->orderref();
	boost::unordered_map<string, CPortfolioOrderPlacer*>::iterator iter 
		= m_workingOrderPlacers.find(orderRef);
	if(iter != m_workingOrderPlacers.end())
	{
		(iter->second)->OnOrderReturned(rtnOrder);
	}
	else
	{
		logger.Warning(boost::str(boost::format("Unexpected order(ref:%s) returned!") % orderRef));
	}
}



