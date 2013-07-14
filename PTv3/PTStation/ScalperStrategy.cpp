#include "StdAfx.h"
#include "ScalperStrategy.h"
#include "ScalperTrigger.h"
#include "Portfolio.h"
#include "globalmembers.h"
#include "DoubleCompare.h"

#include <math.h>

CScalperStrategy::CScalperStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
	, m_diff(0)
	, m_prevAsk(0), m_prevBid(0)
	, m_ask(0), m_askSize(0)
	, m_bid(0), m_bidSize(0)
{
	m_priceTick = strategyItem.sc_pricetick();
	m_caseLE2Tick = strategyItem.sc_casele2tick();
	m_caseLE3Tick = strategyItem.sc_casele3tick();
	m_caseGE4Tick = strategyItem.sc_casege4tick();
	m_caseNoChange = strategyItem.sc_casenochange();
	m_stopLossStrategy = strategyItem.sc_stoplossstrategy();

	CreateTriggers(strategyItem);
}


CScalperStrategy::~CScalperStrategy(void)
{
}

void CScalperStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	if(strategyItem.triggers_size() > 0)
	{
		m_triggers.push_back(TriggerPtr(new CScalperTrigger(strategyItem.triggers(0))));
	}
}

void CScalperStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	m_diff = pQuote->ask() - pQuote->bid();
	m_ask = pQuote->ask();
	m_askSize = pQuote->ask_size();
	m_bid = pQuote->bid();
	m_bidSize = pQuote->bid_size();

	if(m_askSize > 0 && m_bidSize > 0 && IsRunning())
	{
		CPortfolioOrderPlacer* pOrderPlacer = pPortfolio->OrderPlacer();
		if(!(pOrderPlacer->IsWorking()))
		{
			if(m_triggers[0]->Test(m_diff))
			{
				// open position
				trade::PosiDirectionType direction = GetTradeDirection();
#ifdef LOG_FOR_TRADE
				logger.Info(boost::str(boost::format("[%s] Ask: %.2f => %.2f, Bid: %.2f => %.2f, Ask size VS Bid size: %d vs %d")
					% (direction > trade::NET ? (direction == trade::LONG ? "LONG" : "SHORT") : "IGNORE") 
					% m_prevAsk % m_ask % m_prevBid % m_bid % m_askSize % m_bidSize));
#endif
				if(direction > trade::NET)
				{
					double lmtPrice[2];
					if(direction == trade::LONG)
					{
						lmtPrice[0] = m_bid + m_priceTick;
						lmtPrice[1] = m_ask - m_priceTick;
					}
					else // Sell
					{
						lmtPrice[0] = m_ask - m_priceTick;
						lmtPrice[1] = m_bid + m_priceTick;
					}
					pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);
				}
			}
		}
		else
		{
			pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		}
	}

	m_prevAsk = m_ask;
	m_prevBid = m_bid;

	// there is only ONE leg for scalper strategy 
	const LegPtr& leg = pPortfolio->Legs().at(0);
	leg->UpdateLast(pQuote->last());
	leg->UpdateAsk(pQuote->ask());
	leg->UpdateAskSize(pQuote->ask_size());
	leg->UpdateBid(pQuote->bid());
	leg->UpdateBidSize(pQuote->bid_size());
	leg->UpdateTimestamp();
}

void CScalperStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	pPortfUpdateItem->set_sc_diff(m_diff);
}

int CScalperStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double ord_profit = CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);
	
	return totalOpenTimes;
}

double CScalperStrategy::CalcOrderProfit( const trade::MultiLegOrder& openOrder )
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

trade::PosiDirectionType CScalperStrategy::GetTradeDirection()
{
	double askDiff = fabs(m_ask - m_prevAsk);
	double bidDiff = fabs(m_bid - m_prevBid);

	double absDiffDiff = fabs(askDiff - bidDiff);

	if(m_ask > m_prevAsk && m_bid > m_prevBid)
	{
		return trade::LONG;
	}
	else if(m_ask < m_prevAsk && m_bid < m_prevBid)
	{
		return trade::SHORT;
	}
	else if(DoubleEqual(m_bid, m_prevBid) && DoubleEqual(m_ask, m_prevAsk))
	{
		return CalcTradeDirection(m_askSize, m_bidSize, askDiff, bidDiff, m_caseNoChange);
	}
	else if(DoubleLessEqual(absDiffDiff, m_priceTick * 2))
	{
		return CalcTradeDirection(m_askSize, m_bidSize, askDiff, bidDiff, m_caseLE2Tick);
	}
	else if(DoubleLessEqual(absDiffDiff, m_priceTick * 3))
	{
		return CalcTradeDirection(m_askSize, m_bidSize, askDiff, bidDiff, m_caseLE3Tick);
	}
	else if(DoubleGreaterEqual(absDiffDiff, m_priceTick * 4))
	{
		return CalcTradeDirection(m_askSize, m_bidSize, askDiff, bidDiff, m_caseGE4Tick);
	}
	else
	{
		return m_askSize < m_bidSize ? trade::SHORT : trade::LONG;
	}
}

trade::PosiDirectionType CScalperStrategy::CalcTradeDirection(int askSize, int bidSize, double askDiff, double bidDiff, entity::DirectionDepends dependsOn)
{
	switch (dependsOn)
	{
	case entity::ON_SMALL_SIZE:
		return askSize < bidSize ? trade::SHORT : trade::LONG;
	case entity::ON_BIG_SIZE:
		return askSize > bidSize ? trade::SHORT : trade::LONG;
	case entity::ON_SMALL_CHANGE:
		return askDiff < bidDiff ? trade::SHORT : trade::LONG;
	case entity::ON_BIG_CHANGE:
		return askDiff > bidDiff ? trade::SHORT : trade::LONG;
	}

	return trade::NET;
}
