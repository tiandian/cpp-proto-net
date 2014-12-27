#include "stdafx.h"
#include "ManualStrategy.h"
#include "Portfolio.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"
#include "DoubleCompare.h"
#include "TechAnalyStrategy.h"

CManualStrategy::CManualStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
	, m_stopGainCondition(entity::GREATER_EQUAL_THAN)
	, m_stopLossCondition(entity::GREATER_THAN)
	, m_stopGainThreshold(10)
	, m_stopLossThreshold(6)
	, m_stopLossType(entity::LOSS_STOP)
	, m_profit(0)
	, m_nearHigh(0)
	, m_nearLow(0)
	, m_fallback(0)
	, m_bounce(0)
	, m_cost(0)
	, m_tmpHigh(0)
	, m_tmpLow(0)
	, m_lastlast(0)
	, m_positionDirection(entity::NET)
{
	Apply(strategyItem, false);
}

CManualStrategy::~CManualStrategy()
{
}

void CManualStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	boost::mutex::scoped_lock l(m_mut);

	m_retryTimes = strategyItem.retrytimes();
	m_positionDirection = strategyItem.positiondirection();
	m_stopGainCondition = strategyItem.stopgaincondition();
	m_stopGainThreshold = strategyItem.stopgainthreshold();
	m_stopLossCondition = strategyItem.stoplosscondition();
	m_stopLossThreshold = strategyItem.stoplossthreshold();
	m_stopLossType = strategyItem.stoplosstype();
}

void CManualStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);

	CStrategy::Test(pQuote, pPortfolio, timestamp);

	//pQuote->set_last(2343.2);
	//pQuote->set_update_time("09:15:00");
	CalcUpdates(pQuote);

	CManualOrderPlacer* pOrderPlacer = dynamic_cast<CManualOrderPlacer*>(pPortfolio->OrderPlacer());
	
	if (pOrderPlacer->IsOnPending())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Manual - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	bool forceOpening = IsForceOpening();
	if (forceOpening)
	{
		if (!pOrderPlacer->IsWorking())
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Manual - Portfolio(%s) Opening position at %s")
				% pPortfolio->InvestorId() % pPortfolio->ID() % pQuote->update_time()));
			OpenPosition(m_positionDirection, pOrderPlacer, pQuote, timestamp, forceOpening);
			return;
		}
		else
		{
			ResetForceOpen();	// ignore and reset unexpected force opening
		}
	}
	
	bool forceClosing = IsForceClosing();
	if (forceClosing)
	{
		if (pOrderPlacer->IsOpened())
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Manual - Portfolio(%s) Manually Close Position at %s")
				% pPortfolio->InvestorId() % pPortfolio->ID() % pQuote->update_time()));
			ClosePosition(pOrderPlacer, pQuote, "手动平仓");
			return;
		}
		else
		{
			ResetForceClose();	// ignore and reset unexpected force closing
		}
	}
	
	if (!IsRunning())
		return;

	if (pOrderPlacer->IsOpened())
	{
		// Test for Stop Gain
		bool stopGain = MeetCondition(m_profit, m_stopGainCondition, m_stopGainThreshold);
		if (stopGain)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Manual - Portfolio(%s) Stop Gain since Profit(%.2f) > Threshold(%.2f)")
				% pPortfolio->InvestorId() % pPortfolio->ID() % m_profit % m_stopGainThreshold));
			ClosePosition(pOrderPlacer, pQuote, boost::str(boost::format("最新价%.2f, 盈利达到%d, 止盈平仓") % pQuote->last() % m_profit).c_str());
			return;
		}

		// Test for Stop Loss
		double stopLossTestVal = GetStopLossTestValue();
		bool stopLoss = MeetCondition(stopLossTestVal, m_stopLossCondition, m_stopLossThreshold);
		if (stopLoss)
		{
			string closeComment;
			if (m_stopLossType == entity::TRAILING_STOP)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Manual - Portfolio(%s) Trailing Stop since Fallback(%.2f) > Threshold(%.2f)")
					% pPortfolio->InvestorId() % pPortfolio->ID() % stopLossTestVal % m_stopLossThreshold));
				closeComment = boost::str(boost::format("最新价%.2f, 回撤达到%d, 平仓") % pQuote->last() % stopLossTestVal);
			}
			else
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Manual - Portfolio(%s) Stop LOSS since Loss(%.2f) > Threshold(%.2f)")
					% pPortfolio->InvestorId() % pPortfolio->ID() % stopLossTestVal % m_stopLossThreshold));
				closeComment = boost::str(boost::format("最新价%.2f, 亏损达到%d, 止损平仓") % pQuote->last() % stopLossTestVal);
			}
			ClosePosition(pOrderPlacer, pQuote, closeComment.c_str());
			return;
		}
	}
	
}

void CManualStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_mu_profit(m_profit);
	pPortfUpdateItem->set_mu_nearhigh(m_nearHigh);
	pPortfUpdateItem->set_mu_nearlow(m_nearLow);
	pPortfUpdateItem->set_mu_fallback(m_fallback);
	pPortfUpdateItem->set_mu_bounce(m_bounce);
}

void CManualStrategy::OpenPosition(entity::PosiDirectionType direction, CManualOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening)
{
	if (direction > entity::NET)
	{
		double lmtPrice[2];
		if (direction == entity::LONG)
		{
			lmtPrice[0] = pQuote->ask();
		}
		else if (direction == entity::SHORT)
		{
			lmtPrice[0] = pQuote->bid();
		}
		lmtPrice[1] = 0.0;

		LOG_DEBUG(logger, boost::str(boost::format("Manual - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = boost::str(boost::format("手动 %s 开仓 @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0]);

		pOrderPlacer->SetMlOrderStatus(openComment);

		pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);
		ResetForceOpen();
	}
}

void CManualStrategy::ClosePosition(CManualOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText)
{
	if (pOrderPlacer != NULL)
	{
		entity::PosiDirectionType posiDirection = pOrderPlacer->PosiDirection();

		double closePx = 0.0;
		if (posiDirection == entity::LONG)
		{
			closePx = pQuote->bid();
		}
		else if (posiDirection == entity::SHORT)
		{
			closePx = pQuote->ask();
		}

		LOG_DEBUG(logger, boost::str(boost::format("Manual - %s Close position @ %.2f (%s)")
			% GetPosiDirectionText(posiDirection) % closePx  % pQuote->update_time()));

		pOrderPlacer->CloseOrder(closePx);

		ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection, true) % closePx));

	}
}

void CManualStrategy::CalcUpdates(entity::Quote* pQuote)
{
	double last = pQuote->last();
	/*
	if (m_lastlast < 0.001)
	{
		m_lastlast = last;
		return;
	}

	if (last > m_lastlast)
	{
		m_nearHigh = last;
		if (m_nearLow > 0)
			m_bounce = last - m_nearLow;
	}
	else if (last < m_lastlast)
	{
		m_nearLow = last;
		if (m_nearHigh > 0)
			m_fallback = m_nearHigh - last;
	}
	*/

	if (m_cost > 0 && m_positionDirection > entity::NET)
	{
		if (m_positionDirection == entity::LONG)
			m_profit = last - m_cost;
		else
			m_profit = m_cost - last;

		if (last > m_nearHigh)
		{
			m_nearHigh = last;
		}
		else
		{
			m_fallback = m_nearHigh - last;
		}

		if (m_nearLow < 0.001 || last < m_nearLow)
		{
			m_nearLow = last;
		}
		else
		{
			m_bounce = last - m_nearLow;
		}
	}
}

bool CManualStrategy::MeetCondition(double profit, entity::CompareCondition condition, double target)
{
	switch (condition)
	{
	case entity::GREATER_THAN:
		return profit > target;
	case entity::GREATER_EQUAL_THAN:
		return DoubleGreaterEqual(profit, target);
	case entity::LESS_THAN:
		return profit < target;
	case entity::LESS_EQUAL_THAN:
		return DoubleLessEqual(profit, target);
	default:
		return false;
	}
}

int CManualStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	ResetUpdates();

	return totalOpenTimes;
}

void CManualStrategy::ResetUpdates()
{
	m_profit = 0;
	m_nearHigh = 0;
	m_nearLow = 0;
	m_fallback = 0;
	m_bounce = 0;
}

void CManualStrategy::OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
{
	// double confirm
	if (sendingIdx == 0 && offset == trade::OF_OPEN)
	{
		m_cost = price;
	}
	else
		m_cost = 0; // reset m_cost
}

double CManualStrategy::GetStopLossTestValue()
{
	if (m_stopLossType == entity::TRAILING_STOP)
	{
		if (m_positionDirection == entity::LONG)
			return m_fallback;
		else if (m_positionDirection == entity::SHORT)
			return m_bounce;
	}

	return -m_profit;
}
