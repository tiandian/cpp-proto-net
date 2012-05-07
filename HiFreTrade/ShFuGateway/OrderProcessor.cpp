#include "StdAfx.h"
#include "OrderProcessor.h"
#include "MarketAgent.h"
#include "TradeAgent.h"
#include "ClientAgent.h"
#include "LogManager.h"
#include "ReturnOrder.h"
#include "Trade.h"
#include "InputOrderAction.h"
#include "TimeNSalePacket.h"

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>


using namespace std;

extern CMarketAgent g_marketAgent;
extern CClientAgent g_clientAgent;
extern CTradeAgent g_tradeAgent;
extern CLogManager	logger;

COrderProcessor::COrderProcessor(void):
	m_bufferRunner(boost::bind(&COrderProcessor::ProcessQuote, this, _1)),
	m_currentSymbols(1),
	m_currentRecord(new COperationRecordData)
{
}


COrderProcessor::~COrderProcessor(void)
{
	m_bufferRunner.Stop();
}

void COrderProcessor::OnSubscribeCompleted()
{

}

void COrderProcessor::OnUnsubscribeCompleted()
{

}

void COrderProcessor::OnQuoteReceived( CQuote* pQuote )
{
	boost::shared_ptr<CQuote> wrappedQuote(pQuote);
	m_bufferRunner.Enqueue(wrappedQuote);
}

void COrderProcessor::ForwardQuote( boost::shared_ptr<CQuote>& pQuote )
{
	boost::shared_ptr<CMessage> msgPack = pQuote;
	g_clientAgent.Publish(msgPack);
}


void COrderProcessor::SetSymbol( const std::string& symb )
{
	if(!m_currentSymbols[0].empty())
	{
		g_marketAgent.UnSubscribesQuotes(m_currentSymbols);
	}
	m_currentSymbols[0] = symb;
	if(!symb.empty())
	{
		bool subSucc = g_marketAgent.SubscribesQuotes(m_currentSymbols);
		m_latestQuote.setSymbol(symb);
	}
}

void COrderProcessor::ProcessQuote( boost::shared_ptr<CQuote>& pQuote )
{
	// update lastest quote
	m_latestQuote.UpdateQuote(pQuote->get_last(), pQuote->get_ask(), pQuote->get_bid());
	// TODO: test condition and fire trigger


	ForwardQuote(pQuote);
}

void COrderProcessor::Initialize()
{
	g_marketAgent.SetCallbackHanlder(this);

	m_bufferRunner.Start();

	logger.Info("Quote aggregator initialized.");
}

const char* COrderProcessor::NextOrderRef()
{
	boost::lock_guard<boost::mutex> lock(m_mutOrderRef);
	++m_orderRefID;
	sprintf(ORDER_REF_BUF, "%d", m_orderRefID);
	return ORDER_REF_BUF;
}

char OFFSET_FLAG[2][2] = { {OF_OPEN, '\0'}, {OF_CLOSE_TODAY, '\0'} };

std::string OPEN_OFFSET(OFFSET_FLAG[0]);
std::string CLOSE_OFFSET(OFFSET_FLAG[1]);

boost::shared_ptr<CInputOrder> COrderProcessor::CreateOrder(int quantity, 
	OP::OPEN_CLOSE_FLAG open_close, OP::LONG_SHORT_FLAG long_short, double limitPrice)
{
	boost::shared_ptr<CInputOrder> order(new CInputOrder);
	//order->set_brokerid("0240");
	//order->set_investorid("0240050002");
	order->set_instrumentid(m_currentSymbols[0]);
	order->set_orderref(NextOrderRef());

	if(open_close == OP::OPEN)
	{
		// in case wanna open position
		if(long_short == OP::LONG)
		{
			// open long position
			order->set_direction(BUY);
		}
		else if(long_short == OP::SHORT)
		{
			order->set_direction(SELL);
		}
		else
		{
			throw std::exception("unexpected long_short type");
		}
		order->set_comboffsetflag(OPEN_OFFSET);
	}
	else if(open_close == OP::CLOSE)
	{
		// in case wanna close position
		if(long_short == OP::LONG)
		{
			order->set_direction(SELL);
		}
		else if(long_short == OP::SHORT)
		{
			order->set_direction(BUY);
		}
		else
		{
			throw std::exception("unexpected leg side");
		}
		order->set_comboffsetflag(CLOSE_OFFSET);
	}
	else
	{
		// unexpected to be here, return NULL
		return boost::shared_ptr<CInputOrder>();
	}
	
	order->set_limitprice(limitPrice);
	order->set_orderpricetype(LIMIT_PRICE);

	static char CombHedgeFlag[] = { static_cast<char>(SPECULATION) };
	order->set_combhedgeflag(std::string(CombHedgeFlag));

	// 	order->set_limitprice(0);
	order->set_volumetotaloriginal(quantity);

	if(order->orderpricetype() == ANY_PRICE)
	{
		// IOC needed for market price
		order->set_timecondition(TC_IOC);
	}
	else
	{
		order->set_timecondition(TC_GFD);
	}

	order->set_volumecondition(VC_AV);
	order->set_minvolume(1);

	order->set_contingentcondition(IMMEDIATELY);
	order->set_forceclosereason(NOT_FORCE_CLOSE);
	order->set_isautosuspend(false);
	order->set_userforceclose(false);

	return order;
}

void COrderProcessor::OpenPosition( int quantity, int longshort )
{
	if(m_currentSymbols[0].empty())
		return;

	if(m_currentRecord->EntryStatus() == UNOPEN)
	{
		OP::LONG_SHORT_FLAG flag = longshort == SHORT_OPEN ? OP::SHORT : OP::LONG;
		double limitPrice = flag == OP::SHORT ? m_latestQuote.Bid() : m_latestQuote.Ask();
		boost::shared_ptr<CInputOrder> order = CreateOrder(quantity, OP::OPEN, flag, limitPrice);

		m_currentRecord->SetEntryReason(MANUAL_OPEN);
		g_tradeAgent.SubmitOrder(order.get());
	}
}

void COrderProcessor::ClosePosition()
{
	if(m_currentSymbols[0].empty())
		return;

	if(m_currentRecord->EntryStatus() == FULL_FILLED)
	{
		OP::LONG_SHORT_FLAG flag = m_currentRecord->EntryType() == SHORT_OPEN ? OP::SHORT : OP::LONG;
		int quantity = m_currentRecord->EntryQuantity();
		double limitPrice = flag == OP::SHORT ? m_latestQuote.Ask() : m_latestQuote.Bid();
		boost::shared_ptr<CInputOrder> order = CreateOrder(quantity, OP::CLOSE, flag, limitPrice);

		m_currentRecord->SetExitReason(MANUAL_CLOSE);
		g_tradeAgent.SubmitOrder(order.get());
	}
}

void COrderProcessor::OnRspUserLogin( bool succ, std::string& msg, int initOrderRefID )
{
	if(succ){
		m_orderRefID = initOrderRefID;
	}
}

void COrderProcessor::OnRspOrderInsert( bool succ, const std::string& orderRef, const std::string& msg )
{
	if(!succ)
	{
		logger.Info(boost::str(boost::format("Order(%s) submit encounter error:%s") % orderRef.c_str() % msg.c_str()));
	}
}

void COrderProcessor::OnRspOrderAction( bool succ, const std::string& orderRef, const std::string& msg )
{

}

int ConvertToOrderStatusConst(OrderSubmitStatusType submitStatus, OrderStatusType orderStatus)
{
	if(submitStatus == INSERT_SUBMITTED)
	{
		if(orderStatus == ALL_TRADED)
			return FULL_FILLED;
		else if(orderStatus == NO_TRADE_QUEUEING || orderStatus == NO_TRADE_NOT_QUEUEING)
			return PENDING;
		else if(orderStatus == PART_TRADED_QUEUEING || orderStatus == PART_TRADED_NOT_QUEUEING)
			return PARTIALLY_FILLED;
		else
			return ORDER_SUBMIT;
	}
	if(submitStatus == CANCEL_SUBMITTED)
	{
		if(orderStatus == ORDER_CANCELED)
			return CANCELED;
		else
			return UNKNOWN;
	}
	if(submitStatus == ACCEPTED)
	{
		if(orderStatus == ALL_TRADED)
			return FULL_FILLED;
		else if(orderStatus == NO_TRADE_QUEUEING || orderStatus == NO_TRADE_NOT_QUEUEING)
			return PENDING;
		else if(orderStatus == PART_TRADED_QUEUEING || orderStatus == PART_TRADED_NOT_QUEUEING)
			return PARTIALLY_FILLED;
		else if(orderStatus == ORDER_CANCELED)
			return CANCELED;
		else
			return UNKNOWN;
	}
	else if(submitStatus >= INSERT_REJECTED)
	{
		return REJECTED;
	}
	else
	{
		return UNKNOWN;
	}
}

void COrderProcessor::OnRtnOrder( CReturnOrder* order )
{
	if(order->comboffsetflag() == OPEN_OFFSET)
	{
		// Open position order
		int entryType = order->direction() == BUY ? LONG_OPEN : SHORT_OPEN;
		m_currentRecord->SetEntryType(entryType);
		m_currentRecord->SetEntryTime(order->inserttime().c_str());
		m_currentRecord->SetEntryPoint(order->limitprice());

		m_currentRecord->SetEntryQuantity(order->volumetraded());
		int entryStatus = ConvertToOrderStatusConst(order->ordersubmitstatus(), order->orderstatus());
		m_currentRecord->SetEntryStatus(entryStatus);

	}
	else
	{
		// Close position order
		int exitType = order->direction() == SELL ? LONG_CLOSE : SHORT_CLOSE;
		m_currentRecord->SetExitType(exitType);
		m_currentRecord->SetExitTime(order->inserttime().c_str());
		m_currentRecord->SetExitPoint(order->limitprice());

		m_currentRecord->SetExitQuantity(order->volumetraded());
		int exitStatus = ConvertToOrderStatusConst(order->ordersubmitstatus(), order->orderstatus());
		m_currentRecord->SetExitStatus(exitStatus);

	}

	boost::shared_ptr<CMessage> msgPack = m_currentRecord;
	g_clientAgent.Publish(msgPack);
}

void COrderProcessor::OnRtnTrade( CTrade* pTrade )
{
	boost::shared_ptr<CTimeNSalePacket> tnsPacket(new CTimeNSalePacket);
	tnsPacket->SetSymbol(pTrade->instrumentid().c_str());
	tnsPacket->SetTimeStamp(pTrade->tradetime().c_str());
	tnsPacket->SetQuantity(pTrade->volume());
	tnsPacket->SetPrice(pTrade->price());

	int direction = 0;
	if(pTrade->offsetflag() == OF_OPEN)
	{
		if(pTrade->direction() == BUY)
			direction = LONG_OPEN;
		else
			direction = SHORT_OPEN;
	}
	else
	{
		if(pTrade->direction() == SELL)
			direction = LONG_CLOSE;
		else
			direction = SHORT_CLOSE;
	}
	tnsPacket->SetDirection(direction);

	boost::shared_ptr<CMessage> msgPack = tnsPacket;
	g_clientAgent.Publish(msgPack);
}
