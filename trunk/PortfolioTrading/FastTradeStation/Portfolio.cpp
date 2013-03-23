#include "Portfolio.h"
#include "StrategyFactory.h"
#include "DiffStrategy.h"
#include "globalmembers.h"


#include <boost/foreach.hpp>
#include <boost/format.hpp>

enum DIFF_TYPE 
{
	LAST_DIFF, LONG_DIFF, SHORT_DIFF
};

boost::posix_time::ptime CLeg::m_stEpochTime(boost::gregorian::date(2000, 1, 1)); 

double CalcDiff(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	// calculate the diff
	double diff = 0;
	for(vector<LegPtr>::iterator iter = legs.begin(); iter != legs.end(); ++iter)
	{
		entity::PosiDirectionType legSide = (*iter)->Side();
		double legPrice = 0;
		if(legSide == entity::LONG)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legPrice = (*iter)->Ask();
				break;
			case SHORT_DIFF:
				legPrice = (*iter)->Bid();
				break;
			case LAST_DIFF:
			default:
				legPrice = (*iter)->Last();
			}
		}
		else if(legSide == entity::SHORT)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legPrice = (*iter)->Bid();
				break;
			case SHORT_DIFF:
				legPrice = (*iter)->Ask();
				break;
			case LAST_DIFF:
			default:
				legPrice = (*iter)->Last();
			}
		}

		if(legPrice > 0)
		{
			if((*iter)->Side() == entity::LONG)
			{
				diff +=	legPrice;
			}
			else
				diff -= legPrice;
		}
		else	// if one of legs has no price, set diff 0 anyway
			diff = 0;
	}
	return diff;
}

double CalcDiff(vector<LegPtr>& legs)
{
	// calculate the diff
	double diff = 0;
	for(vector<LegPtr>::iterator iter = legs.begin(); iter != legs.end(); ++iter)
	{
		double legPrice = (*iter)->Last();
		if(legPrice > 0)
		{
			if((*iter)->Side() == entity::LONG)
			{
				diff +=	legPrice;
			}
			else
				diff -= legPrice;
		}
		else	// if one of legs has no price, set diff 0 anyway
			diff = 0;
	}
	return diff;
}

double CalcPortfProfit(trade::MultiLegOrder* openOrder, trade::MultiLegOrder* closeOrder)
{
	assert(openOrder != NULL);
	assert(closeOrder != NULL);

	double totalProfit = 0;
	int legCount = openOrder->legs_size();
	for(int i = 0; i < legCount; ++i)
	{
		const trade::Order& oo = openOrder->legs(i);
		const trade::Order& co = closeOrder->legs(i);
		trade::TradeDirectionType direction = oo.direction();
		if(direction == trade::BUY)
		{
			totalProfit += co.limitprice() - oo.limitprice();
		}
		else if(direction == trade::SELL)
		{
			totalProfit += oo.limitprice() - co.limitprice();
		}
	}

	return totalProfit;
}

int CalcSize(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	// calculate the diff
	int diffSize = 0;
	for(vector<LegPtr>::iterator iter = legs.begin(); iter != legs.end(); ++iter)
	{
		int legSize = 0;
		entity::PosiDirectionType legSide = (*iter)->Side();
		if(legSide == entity::LONG)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legSize = (*iter)->AskSize();
				break;
			case SHORT_DIFF:
				legSize = (*iter)->BidSize();
				break;
			}
		}
		else if(legSide == entity::SHORT)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legSize = (*iter)->BidSize();
				break;
			case SHORT_DIFF:
				legSize = (*iter)->AskSize();
				break;
			}
		}

		if(legSize > 0)
		{
			if(diffSize == 0) diffSize = legSize;
			else
				diffSize = legSize < diffSize ? legSize : diffSize;
		}
	}
	return diffSize;
}

CPortfolio::CPortfolio(void):
m_pClientAgent(NULL),
m_openedOrderCount(0),
m_selfClose(false),
m_isPlacingOrder(false)
{
}


CPortfolio::~CPortfolio(void)
{
	Cleanup();
}

CLeg* CPortfolio::AddLeg(entity::LegItem* legItem)
{
	int legCount = m_vecLegs.size();
	LegPtr leg(new CLeg(legCount + 1));
	leg->SetInnerItem(legItem);
	m_vecLegs.push_back(leg);
	return leg.get();
}

int CPortfolio::Count()
{
	return m_vecLegs.size();
}

CLeg* CPortfolio::GetLeg( int legId )
{
	if(legId <= (int)m_vecLegs.size())
		return m_vecLegs[legId - 1].get();

	return NULL;
}

CLeg* CPortfolio::GetLeg(const string& symbol)
{
	// update last
	for(vector<LegPtr>::iterator iter = m_vecLegs.begin(); iter != m_vecLegs.end(); ++iter)
	{
		if((*iter)->Symbol() == symbol)
		{
			return iter->get();
		}
	}

	return NULL;
}

void CLeg::SetInnerItem(entity::LegItem* pItem)
{
	m_pInnerItem = pItem;
}

void CLeg::UpdateTimestamp()
{
	boost::unique_lock<boost::mutex> l(m_mut);
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration diff = now - m_stEpochTime;
	m_timestamp = diff.total_milliseconds();
	m_condQuoteUpdated.notify_one();
}

bool CLeg::CompareTimestamp(long destTs)
{
	return m_timestamp > destTs;
}

bool CLeg::IsQuoteUpdated(long timestamp)
{
	boost::unique_lock<boost::mutex> l(m_mut);
	if(m_condQuoteUpdated.timed_wait(l, boost::posix_time::seconds(3), 
		boost::bind(&CLeg::CompareTimestamp, this, timestamp)))
	{
		return true;
	}
	return false;
}

void CPortfolio::SetItem(CClientAgent* pClient, entity::PortfolioItem* pPortfItem )
{
	m_pClientAgent = pClient;
	m_innerItem = PortfItemPtr(pPortfItem);
	
	// backup current legs given by initialization
	google::protobuf::RepeatedPtrField< ::entity::LegItem > legs = pPortfItem->legs();
	
	// clear generated legs and set my own legs that works normally
	pPortfItem->clear_legs();

	for(google::protobuf::RepeatedPtrField< ::entity::LegItem >::iterator iter = legs.begin();
		iter != legs.end(); ++iter)
	{
		entity::LegItem* nl = pPortfItem->add_legs();
		nl->set_symbol(iter->symbol());
		nl->set_ratio(iter->ratio());
		nl->set_side(iter->side());
		nl->set_last(iter->last());
		nl->set_ask(iter->ask());
		nl->set_asksize(iter->asksize());
		nl->set_bid(iter->bid());
		nl->set_bidsize(iter->bidsize());
		nl->set_ispreferred(iter->ispreferred());
		nl->set_status(iter->status());

		AddLeg(nl);
	}

	boost::shared_ptr<CDiffStrategy> strategy(CreateStrategy(this, pPortfItem->strategyname(), pPortfItem->strategydata()));
	strategy->Client(pClient);
	
	strategy->SetAutoOpen(pPortfItem->autoopen());
	strategy->SetStopGain(pPortfItem->autostopgain());
	strategy->SetStopLoss(pPortfItem->autostoploss());
	strategy->SetAutoTracking(pPortfItem->autotrack());
	strategy->SetEnablePrefer(pPortfItem->enableprefer());
	m_strategy = strategy;
}

void CPortfolio::OnQuoteRecevied( entity::Quote* pQuote )
{
	// update last
	for(vector<LegPtr>::iterator iter = m_vecLegs.begin(); iter != m_vecLegs.end(); ++iter)
	{
		if((*iter)->Symbol() == pQuote->symbol())
		{
			(*iter)->UpdateLast(pQuote->last());
			(*iter)->UpdateAsk(pQuote->ask());
			(*iter)->UpdateAskSize(pQuote->ask_size());
			(*iter)->UpdateBid(pQuote->bid());
			(*iter)->UpdateBidSize(pQuote->bid_size());
			(*iter)->UpdateTimestamp();
			break;
		}
	}

	if(m_vecLegs.size() > 1)
	{
		double lastDiff = CalcDiff(m_vecLegs, LAST_DIFF);
		m_innerItem->set_diff(lastDiff);
		double longDiff = CalcDiff(m_vecLegs, LONG_DIFF);
		m_innerItem->set_longdiff(longDiff);
		int longDiffSize = CalcSize(m_vecLegs, LONG_DIFF);
		m_innerItem->set_longsize(longDiffSize);
		double shortDiff = CalcDiff(m_vecLegs, SHORT_DIFF);
		m_innerItem->set_shortdiff(shortDiff);
		int shortDiffSize = CalcSize(m_vecLegs, SHORT_DIFF);
		m_innerItem->set_shortsize(shortDiffSize);
	}
	else	// only one leg
	{
		m_innerItem->set_diff(pQuote->ask() - pQuote->bid());
		m_innerItem->set_longdiff(pQuote->ask());
		m_innerItem->set_longsize(pQuote->ask_size());
		m_innerItem->set_shortdiff(pQuote->bid());
		m_innerItem->set_shortsize(pQuote->bid_size());
	}

	m_strategy->Test();
	
	PushUpdate();
}

void CPortfolio::SubscribeQuotes()
{
	BOOST_FOREACH(const LegPtr& leg, m_vecLegs)
	{
		m_listenSymbols.push_back(leg->Symbol());
	}

	m_pClientAgent->RegQuote(m_listenSymbols);
}

void CPortfolio::Cleanup()
{
	if(m_pClientAgent != NULL)
	{
		m_pClientAgent->UnregQuote(m_listenSymbols);
		m_listenSymbols.clear();
	}
}

void CPortfolio::PushUpdate()
{
	m_pClientAgent->OnPortfolioUpdated(m_innerItem.get());
}

int CPortfolio::NewOrderId(string& newId)
{
	boost::mutex::scoped_lock lock(m_mut);
	newId = boost::str(boost::format("%s-%d") % ID().c_str() % ++m_openedOrderCount);
	return m_openedOrderCount;
}

void CPortfolio::ApplyStrategySetting( const string& name, const string& data )
{
	logger.Info(
		boost::str(
			boost::format("Portfolio (%s) apply %s settings") % ID().c_str() % name.c_str()));
	m_strategy->ApplySettings(data);
}

CPortfolio* CPortfolio::Create( CClientAgent* pClient, entity::PortfolioItem* pPortfItem )
{
	CPortfolio* pPortf = new CPortfolio();
	pPortf->SetItem(pClient, pPortfItem);
	return pPortf;
}

void CPortfolio::TurnSwitches( bool isAutoOpen, bool isAutoStopGain, bool isAutoStopLoss, bool isAutoTracking, bool enablePrefer )
{
	m_strategy->SetAutoOpen(isAutoOpen);
	m_strategy->SetStopGain(isAutoStopGain);
	m_strategy->SetStopLoss(isAutoStopLoss);
	m_strategy->SetAutoTracking(isAutoTracking);
	m_strategy->SetEnablePrefer(enablePrefer);

	logger.Info(boost::str(boost::format("Portf (%s): Open %d, StopGain %d, StopLoss %d, AutoTrack %d, Prefer %d")
		% ID().c_str() % isAutoOpen % isAutoStopGain % isAutoStopLoss % isAutoTracking % enablePrefer));
}

void CPortfolio::EnableStrategy( bool isEnabled )
{
	if(isEnabled)
	{
		logger.Info(boost::str(boost::format("Portf (%s) START strategy >>>") % ID().c_str()));
		m_strategy->Start();
	}
	else
	{
		logger.Info(boost::str(boost::format("Portf (%s) STOP strategy <<<") % ID().c_str()));
		m_strategy->Stop();
	}
	m_innerItem->set_strategyrunning(isEnabled);
}

double CPortfolio::CalcScalpeOrderProfit(const MultiLegOrderPtr& openOrder)
{
	double profit = 0;
	int legCount = openOrder->legs_size();
	for(int ordIdx = 0; ordIdx < legCount; ++ordIdx)
	{
		const trade::Order& legOrd = openOrder->legs(ordIdx);
		double ordPrice = legOrd.limitprice();
		if(ordPrice > 0)
		{
			if(legOrd.direction() == trade::BUY)
			{
				profit -= ordPrice;
			}
			else
			{
				profit += ordPrice;
			}
		}
	}
	return profit;
}

void CPortfolio::AddPosition( const MultiLegOrderPtr& openOrder )
{
	int qty = openOrder->quantity();
	
	if(openOrder->reason() == trade::SR_Scalpe)
	{
		double ord_profit = CalcScalpeOrderProfit(openOrder);
		AddProfit(ord_profit);
		
		IncrementalOpenCloseTimes(qty);
	}
	else
	{
		double cost = CalcMlOrderCost(openOrder);
		int origQty = PositionQuantity();

		double newAvgCost = (AvgCost() * origQty + cost * qty) / (origQty + qty);
		AvgCost(newAvgCost);
		IncrementalOpenTimes(qty);
	}
}

void CPortfolio::RemovePosition( const MultiLegOrderPtr& closeOrder )
{
	int qty = closeOrder->quantity();
	double cost = CalcMlOrderCost(closeOrder);

	double orderProfit = (cost - AvgCost()) * qty;
	AddProfit(orderProfit);

	int origQty = PositionQuantity();
	int remaing = origQty - qty;
	if(remaing > 0)
	{
		double newAvgCost = (AvgCost() * origQty - cost * qty) / remaing;
		AvgCost(newAvgCost);
	}
	else
	{
		AvgCost(0);
	}

	IncrementalCloseTimes(qty);
}

double CPortfolio::CalcMlOrderCost( const MultiLegOrderPtr& openOrder )
{
	double cost = 0;
	int legCount = openOrder->legs_size();
	for(int ordIdx = 0; ordIdx < legCount; ++ordIdx)
	{
		const trade::Order& legOrd = openOrder->legs(ordIdx);
		double ordPrice = legOrd.limitprice();
		if(ordPrice > 0)
		{
			CLeg* pLeg = GetLeg(legOrd.instrumentid());
			if(pLeg != NULL)
			{
				if(pLeg->Side() == entity::LONG)
				{
					cost +=	ordPrice;
				}
				else
					cost -= ordPrice;
			}
		}
	}

	return cost;
}

bool CPortfolio::EnablePrefer()
{
	return m_strategy->EnablePrefer();
}

bool CPortfolio::AutoTracking()
{
	return m_strategy->IsAutoTracking();
}

