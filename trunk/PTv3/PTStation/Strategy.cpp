#include "StdAfx.h"
#include "Strategy.h"
#include "Portfolio.h"
#include "SettingChangeTrace.h"

CStrategy::CStrategy(const entity::StrategyItem& strategyItem)
	: m_running(false)
	, m_forceOpening(false)
	, m_forceClosing(false)
	, m_retryTimes(0)
	, m_openTimeout(0)
{
	m_type = strategyItem.type();
}

CStrategy::~CStrategy(void)
{
}

void CStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
#ifdef LOG_FOR_TRADE
	if(withTriggers)
	{
		PrintIntegerChange("Retry Times", m_retryTimes, strategyItem.retrytimes());
		PrintIntegerChange("Open timeout", m_openTimeout, strategyItem.opentimeout());
	}
#endif

	m_retryTimes = strategyItem.retrytimes();
	m_openTimeout = strategyItem.opentimeout();
	if(withTriggers)
		ApplyTrigger(strategyItem);
}

void CStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	vector<LegPtr>& vecLegs = pPortfolio->Legs();
	// update last
	for(vector<LegPtr>::iterator iter = vecLegs.begin(); iter != vecLegs.end(); ++iter)
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
}

void CStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	pPortfUpdateItem->set_running(IsRunning());
	for(int i = 0; i < pPortfUpdateItem->triggers_size(); ++i)
	{
		entity::TriggerStatus* triggerStatus = pPortfUpdateItem->mutable_triggers(i);
		triggerStatus->set_enabled(m_triggers[i]->IsEnabled());
	}
}

int CStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	return pPortfolio->TotalOpenTimes();
}

int CStrategy::OnPortfolioRemovePosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder )
{
	return pPortfolio->TotalCloseTimes();
}

void CStrategy::AddProfit( CPortfolio* pPortfolio, double profit )
{
	pPortfolio->AddProfit(profit);
}

void CStrategy::SetAvgCost( CPortfolio* pPortfolio, double avgCost )
{
	pPortfolio->SetAvgCost(avgCost);
}

int CStrategy::IncrementOpenTimes(  CPortfolio* pPortfolio, int qty )
{
	return pPortfolio->IncrementalOpenTimes(qty);
}

int CStrategy::IncrementCloseTimes(  CPortfolio* pPortfolio, int qty )
{
	return pPortfolio->IncrementalCloseTimes(qty);
}

int CStrategy::PositionQuantity( CPortfolio* pPortfolio )
{
	return pPortfolio->m_currentPosition;
}

double CStrategy::AvgCost( CPortfolio* pPortfolio )
{
	return pPortfolio->AvgCost();
}

void CStrategy::ApplyTrigger( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		m_triggers[i]->Apply(strategyItem.triggers(i));
	}
}

double CStrategy::CalcOrderProfit( const trade::MultiLegOrder& openOrder )
{
	double profit = 0;
	int legCount = openOrder.legs_size();
	for(int ordIdx = 0; ordIdx < legCount; ++ordIdx)
	{
		const trade::Order& legOrd = openOrder.legs(ordIdx);
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