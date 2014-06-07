#include "StdAfx.h"
#include "ArbitrageStrategy.h"
#include "ArbitrageTrigger.h"
#include "PriceBarDataProxy.h"
#include "globalmembers.h"
#include "PortfolioArbitrageOrderPlacer.h"

enum DIFF_TYPE 
{
	LAST_DIFF, LONG_DIFF, SHORT_DIFF
};

double CalcDiff(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	// calculate the diff
	double diff = 0;
	assert(legs.size() > 1);

	if(diffType == LONG_DIFF)
	{
		double long_cost = legs[0]->Ask();
		double short_cost = legs[1]->Bid();
		diff = long_cost - short_cost;
	}
	else if(diffType == SHORT_DIFF)
	{
		double short_cost = legs[0]->Bid();
		double long_cost = legs[1]->Ask();
		diff = short_cost - long_cost;
	}

	return diff;
}

int CalcSize(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	// calculate the diff
	int diffSize = 0;
	assert(legs.size() > 1);

	if(diffType == LONG_DIFF)
	{
		int long_size = legs[0]->AskSize();
		int short_size = legs[1]->BidSize();
		diffSize = long_size < short_size ? long_size : short_size;
	}
	else if(diffType == SHORT_DIFF)
	{
		int short_size = legs[0]->BidSize();
		int long_size = legs[1]->AskSize();
		diffSize = short_size < long_size ? short_size : long_size;
	}
	
	return diffSize;
}

CArbitrageStrategy::CArbitrageStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_timeFrame(60)
	, m_bollPeriod(26)
	, m_stdDevTimes(2)
	, m_lastDiff(0)
	, m_longDiff(0)
	, m_longDiffSize(0)
	, m_shortDiff(0)
	, m_shortDiffSize(0)
	, m_bollTop(0)
	, m_bollBottom(0)
{
	Apply(strategyItem, false);

	CreateTriggers(strategyItem);
}


CArbitrageStrategy::~CArbitrageStrategy(void)
{
}

void CArbitrageStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		TriggerPtr trigger(new CArbitrageTrigger(strategyItem.triggers(i)));
		m_triggers.push_back(trigger);
	}
}

void CArbitrageStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	// TODO
	//m_bollPeriod = strategyItem.ar_bollperiod();
	//m_stdDevTimes = strategyItem.ar_stddevtimes();

	// make sure following parameters having values
	if(m_openTimeout == 0)
		m_openTimeout = 350;
	if(m_retryTimes == 0)
		m_retryTimes = 8;

	if(!withTriggers)
	{
		// don't touch hist data source when editing strategy
		PrepareHistDataSrc(strategyItem);

		// Initialize Indicator set
		const vector<CPriceBarDataProxy*>& dataProxies = DataProxies();
		if(dataProxies.size() > 1)
		{
			m_diffRecordSet = DiffRecordSetPtr(
				new CDiffRecordSet(
				dataProxies[0]->GetOHLCRecordSet(),
				dataProxies[1]->GetOHLCRecordSet()
			));

			m_bollDataSet = BollDataSetPtr(new CBollDataSet(dataProxies[0]->GetRecordSetSize(), 26, 2));
		}
	}
}

void CArbitrageStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);
	vector<LegPtr>& vecLegs = pPortfolio->Legs();

	m_lastDiff = CalcDiff(vecLegs, LAST_DIFF);
	m_longDiff = CalcDiff(vecLegs, LONG_DIFF);
	m_longDiffSize = CalcSize(vecLegs, LONG_DIFF);
	m_shortDiff = CalcDiff(vecLegs, SHORT_DIFF);
	m_shortDiffSize = CalcSize(vecLegs, SHORT_DIFF);

	if(!IsRunning())
		return;

	string symbol = pQuote->symbol();
	COHLCRecordSet* ohlc = GetRecordSet(symbol, m_timeFrame, timestamp);
	if(ohlc == NULL)
		return;

	m_diffRecordSet->Calculate(ohlc);

	m_bollDataSet->Calculate(m_diffRecordSet.get());
	int currentBarIdx = m_diffRecordSet->GetEndIndex();

	m_bollTop = m_bollDataSet->GetRef(IND_TOP, 0);
	m_bollBottom = m_bollDataSet->GetRef(IND_BOTTOM, 0);

	CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsWorking())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Strategy - Check and likely retry submit order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	entity::PosiDirectionType direction = GetTradeDirection();

	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = false;
		bool forceClosing = IsForceClosing();
		if(forceClosing) // This close condition check is only effective on the bar after open
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Forcibly Closing position")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ClosePosition(pOrderPlacer, pPortfolio, pQuote, timestamp, "手动平仓"); 
			return;
		}
		
		// Stop gain
		if(direction != entity::NET)
		{
			if(pOrderPlacer->PosiDirection() != direction)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Closing position due to stop gain")
					% pPortfolio->InvestorId() % pPortfolio->ID()));
				ClosePosition(pOrderPlacer, pPortfolio, pQuote, timestamp, "回头触发止损(盈)");
				return;
			}
		}

		// Stop Loss

		return;
	}

	LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing open direction - longDiff:%.2f vs bottom:%.2f, shortDiff:%.2f vs top:%.2f -->> %s")
		% pPortfolio->InvestorId() % pPortfolio->ID() % m_longDiff % m_bollBottom % m_shortDiff % m_bollTop % GetPosiDirectionText(m_side)));

	bool forceOpening = IsForceOpening();
	if(direction > entity::NET || forceOpening )
	{
		if(!pOrderPlacer->IsWorking())
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Opening position at bar %d")
				% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx ));
			OpenPosition(direction, pOrderPlacer, pPortfolio, pQuote, timestamp, forceOpening);
			return;
		}
	}
	/*
	if(IsRunning())
	{

		for( TriggerIter iter = m_triggers.begin(); iter != m_triggers.end(); ++iter)
		{
			if((*iter)->IsEnabled())
			{
				double diffToTest = m_side == entity::LONG ? m_longDiff : m_shortDiff;
				if((*iter)->Test(diffToTest))
				{
					CPortfolioOrderPlacer* orderPlacer = pPortfolio->OrderPlacer();
					// To do
					// orderPlacer->Run();
					// if one is triggered, don't test other triggers any more
					break;
				}
			}
		}
	}
	*/
}

void CArbitrageStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_ar_diff(m_lastDiff);
	pPortfUpdateItem->set_ar_longdiff(m_longDiff);
	pPortfUpdateItem->set_ar_longsize(m_longDiffSize);
	pPortfUpdateItem->set_ar_shortdiff(m_shortDiff);
	pPortfUpdateItem->set_ar_shortsize(m_shortDiffSize);
}

int CArbitrageStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double cost = CalcMlOrderCost(openOrder);
	int origQty = PositionQuantity(pPortfolio);

	double newAvgCost = (AvgCost(pPortfolio) * origQty + cost * qty) / (origQty + qty);
	SetAvgCost(pPortfolio, newAvgCost);
	return IncrementOpenTimes(pPortfolio, qty);
}

int CArbitrageStrategy::OnPortfolioRemovePosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder )
{
	int qty = closeOrder.quantity();
	double cost = CalcMlOrderCost(closeOrder);

	double orderProfit = (cost - AvgCost(pPortfolio)) * qty;
	AddProfit(pPortfolio, orderProfit);

	int origQty = PositionQuantity(pPortfolio);
	int remaing = origQty - qty;
	if(remaing > 0)
	{
		double newAvgCost = (AvgCost(pPortfolio) * origQty - cost * qty) / remaing;
		SetAvgCost(pPortfolio, newAvgCost);
	}
	else
	{
		SetAvgCost(pPortfolio, 0);
	}

	return IncrementCloseTimes(pPortfolio, qty);
}


double CArbitrageStrategy::CalcMlOrderCost( const trade::MultiLegOrder& openOrder )
{
	double cost = 0;
	int legCount = openOrder.legs_size();
	if(legCount > 1)
	{
		const trade::Order& firstLeg = openOrder.legs(0);
		double firstLegPx = firstLeg.limitprice();

		const trade::Order& secondLeg = openOrder.legs(1);
		double secondLegPx = secondLeg.limitprice();

		cost = firstLegPx - secondLegPx;
	}

	return cost;
}

const string& CArbitrageStrategy::GetAnotherLegSymbol( const string& symb, const vector<LegPtr>& legs )
{
	static string emptySymb;
	BOOST_FOREACH(const LegPtr& l, legs)
	{
		if(l->Symbol() != symb)
			return l->Symbol();
	}
	return emptySymb;
}

entity::PosiDirectionType CArbitrageStrategy::GetTradeDirection()
{
	if(m_longDiff < m_bollBottom)
		return entity::LONG;
	
	if(m_shortDiff > m_bollTop)
		return entity::SHORT;

	return entity::NET;
}

void CArbitrageStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioArbitrageOrderPlacer* pOrderPlacer, CPortfolio* pPortfolio, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening )
{
	if(direction > entity::NET)
	{
		double lmtPrice[2];
		assert(pPortfolio->Count() > 1);
		if(direction == entity::LONG)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Ask();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Bid();
		}
		else if(direction == entity::SHORT)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Bid();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Ask();
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Open position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1] % pQuote->update_time()));

		// TODO feed comment
		//pOrderPlacer->SetMlOrderStatus(openComment);
		pOrderPlacer->OpenPosition(direction, lmtPrice, 2, timestamp);
		m_side = direction;
		//m_costDiff = lmtPrice[0] - lmtPrice[1];
		ResetForceOpen();
	}
}

void CArbitrageStrategy::ClosePosition( CPortfolioArbitrageOrderPlacer* pOrderPlacer, CPortfolio* pPortfolio, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment )
{
	if(pOrderPlacer != NULL)
	{
		entity::PosiDirectionType direction = pOrderPlacer->PosiDirection();

		double lmtPrice[2];
		assert(pPortfolio->Count() > 1);
		if(direction == entity::LONG)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Bid();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Ask();
		}
		else if(direction == entity::SHORT)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Ask();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Bid();
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Close position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1]  % pQuote->update_time()));

		pOrderPlacer->ClosePosition(direction, lmtPrice, 2, timestamp);

		//m_openAtBarIdx = 0; // reset open bar position
		//ResetForceClose();
		//pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s @ %.2f - %.2f")
		//	% comment % GetPosiDirectionText(direction, true) % lmtPrice[0] % lmtPrice[1]));

	}
}



