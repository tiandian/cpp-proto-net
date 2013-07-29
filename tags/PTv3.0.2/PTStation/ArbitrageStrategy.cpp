#include "StdAfx.h"
#include "ArbitrageStrategy.h"
#include "ArbitrageTrigger.h"
#include "Portfolio.h"

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

CArbitrageStrategy::CArbitrageStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
	, m_lastDiff(0)
	, m_longDiff(0)
	, m_longDiffSize(0)
	, m_shortDiff(0)
	, m_shortDiffSize(0)
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
	CStrategy::Apply(strategyItem, withTriggers);

	m_side = strategyItem.ar_side();
}

void CArbitrageStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	CStrategy::Test(pQuote, pPortfolio, timestamp);
	vector<LegPtr>& vecLegs = pPortfolio->Legs();

	m_lastDiff = CalcDiff(vecLegs, LAST_DIFF);
	m_longDiff = CalcDiff(vecLegs, LONG_DIFF);
	m_longDiffSize = CalcSize(vecLegs, LONG_DIFF);
	m_shortDiff = CalcDiff(vecLegs, SHORT_DIFF);
	m_shortDiffSize = CalcSize(vecLegs, SHORT_DIFF);

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



