#include "StdAfx.h"
#include "WMATrendStrategy.h"
#include "WMATrendTrigger.h"
#include "TrailingStopTrigger.h"
#include "Portfolio.h"
#include "PortfolioTrendOrderPlacer.h"
#include "DoubleCompare.h"
#include "PriceBarDataProxy.h"
#include "globalmembers.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"
#include "OHLCRecordSet.h"

CWMATrendStrategy::CWMATrendStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_wmaParam(0)
	, m_maN(0)
	, m_period(0)
	, m_pTrailingStopTrigger(NULL)
	, m_DirectionOpened(entity::NET)
	, m_marketOpen(false)
	, m_openAtBarIdx(0)
{
	m_arrLine[0] = 0.0;
	m_arrLine[1] = 0.0;

	Apply(strategyItem, false);

	CreateTriggers(strategyItem);
}


CWMATrendStrategy::~CWMATrendStrategy(void)
{
}

void CWMATrendStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	m_wmaParam = strategyItem.wt_wmaparam();
	m_maN = strategyItem.wt_man();
	m_period = strategyItem.wt_period();

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
		for(vector<CPriceBarDataProxy*>::const_iterator iter = dataProxies.begin(); iter != dataProxies.end(); ++iter)
		{
			if((*iter)->Precision() == m_period)
			{
				m_trendIndicatorSet = WMATrendSetPtr(new CWMATrendDataSet((*iter)->GetRecordSetSize(), 
					m_wmaParam, m_maN));
			}
		}
	}
}

void CWMATrendStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
/*
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		if(triggerItem.name() == CrossTrailingStopTriggerName)
		{
			m_pTrailingStopTrigger = new CTrailingStopTrigger(triggerItem);
			TriggerPtr trigger(m_pTrailingStopTrigger);
			m_triggers.push_back(trigger);
		}
	}
	*/
}

void CWMATrendStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_wt_fastline(m_arrLine[0]);
	pPortfUpdateItem->set_wt_slowline(m_arrLine[1]);
}

void CWMATrendStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);

	//pQuote->set_last(2343.2);
	//pQuote->set_update_time("09:15:00");

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	if(!IsRunning())
		return;

	if(!m_marketOpen)
	{
		string symbol = pQuote->symbol();
		string quoteUpdateTime = pQuote->update_time();
		bool isIF = isSymbolIF(symbol);
		string targetBeginTime = isIF ? IF_START_1 : NON_IF_START_1;
		if(quoteUpdateTime.compare(targetBeginTime) >= 0)
		{
			m_marketOpen = true;
		}
		else
		{
			return;
		}
	}

	string symbol = pQuote->symbol();

	COHLCRecordSet* slowOHLC = GetRecordSet(symbol, m_period, timestamp);
	if(slowOHLC == NULL)
		return;

	int currentBarIdx = slowOHLC->GetEndIndex();

	m_trendIndicatorSet->Calculate(slowOHLC);

	double fastVal = m_trendIndicatorSet->GetRef(IND_FAST_LINE, 0);
	m_arrLine[0] = fastVal;
	double slowVal = m_trendIndicatorSet->GetRef(IND_SLOW_LINE, 0);
	m_arrLine[1] = slowVal;

	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] WMA Trend - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	entity::PosiDirectionType direction = GetDirection(fastVal, slowVal);

	double prevFastVal = m_trendIndicatorSet->GetRef(IND_FAST_LINE, 1);
	double prevFlowVal = m_trendIndicatorSet->GetRef(IND_SLOW_LINE, 1);
	entity::PosiDirectionType prevDirection = GetDirection(prevFastVal, prevFlowVal);

	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = false;
		bool forceClosing = IsForceClosing();
		if(currentBarIdx > m_openAtBarIdx || forceClosing) // This close condition check is only effective on the bar after open
		{
			meetCloseCondition = m_DirectionOpened != direction;
			if(meetCloseCondition || forceClosing)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] WMA Trend - Portfolio(%s) Closing position due to %s")
					% pPortfolio->InvestorId() % pPortfolio->ID() % (forceClosing ? "Force close" : "fast dead cross")));
				ClosePosition(pOrderPlacer, pQuote, 
					forceClosing ? "手动平仓" : boost::str(boost::format("WMA快线%s叉慢线") 
					% (direction == entity::LONG ? "金" : "死")).c_str());
				return;
			}
		}
		
		if(m_pTrailingStopTrigger != NULL)
		{
			meetCloseCondition = m_pTrailingStopTrigger->Test(pQuote->last());
			if(meetCloseCondition)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] WMA Trend - Portfolio(%s) Closing position due to Trailing stop")
					% pPortfolio->InvestorId() % pPortfolio->ID()));
				ClosePosition(pOrderPlacer, pQuote, "回头触发止损(盈)");
				return;
			}
		}		

		return;	// don't need to go to test open trigger any more
	}
	
	LOG_DEBUG(logger, boost::str(boost::format("[%s] WMA Trend - Portfolio(%s) Testing open direction - fast line:%.2f vs slow line:%.2f -->> %s")
		% pPortfolio->InvestorId() % pPortfolio->ID() % fastVal % slowVal % GetPosiDirectionText(direction)));
	bool forceOpening = IsForceOpening();
	if(direction > entity::NET && 
		(forceOpening || direction != prevDirection))
	{
		if(!pOrderPlacer->IsWorking())
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] WMA Trend - Portfolio(%s) Opening position at bar %d")
				% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx ));
			OpenPosition(direction, pOrderPlacer, pQuote, timestamp, forceOpening);
			m_openAtBarIdx = currentBarIdx;
			return;
		}
	}

}

void CWMATrendStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening )
{
	if(direction > entity::NET)
	{
		double lmtPrice[2];
		if(direction == entity::LONG)
		{
			lmtPrice[0] = pQuote->ask();
		}
		else if(direction == entity::SHORT)
		{
			lmtPrice[0] = pQuote->bid();
		}
		lmtPrice[1] = 0.0;

		LOG_DEBUG(logger, boost::str(boost::format("WMA Trend - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = forceOpening ? 
			boost::str(boost::format("手动 %s 开仓 @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0])
			: 
			boost::str(boost::format("WMA快线%s叉慢线 - %s 开仓 @ %.2f")
			% (direction == entity::LONG ? "金" : "死") % GetPosiDirectionText(direction) % lmtPrice[0]);

		pOrderPlacer->SetMlOrderStatus(openComment);

		pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);

		m_DirectionOpened = direction;
		ResetForceOpen();
		if(m_pTrailingStopTrigger != NULL)
		{
			m_pTrailingStopTrigger->Enable(lmtPrice[0], direction);
		}
	}
}

void CWMATrendStrategy::ClosePosition( CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText )
{
	if(pOrderPlacer != NULL)
	{
		entity::PosiDirectionType posiDirection = pOrderPlacer->PosiDirection();

		double closePx = 0.0;
		if(posiDirection == entity::LONG)
		{
			closePx = pQuote->bid();
		}
		else if(posiDirection == entity::SHORT)
		{
			closePx = pQuote->ask();
		}

		LOG_DEBUG(logger, boost::str(boost::format("WMA Trend - %s Close position @ %.2f (%s)")
			% GetPosiDirectionText(posiDirection) % closePx  % pQuote->update_time()));

		pOrderPlacer->CloseOrder(closePx);

		m_openAtBarIdx = 0; // reset open bar position
		ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection, true) % closePx));

	}
}

entity::PosiDirectionType CWMATrendStrategy::GetDirection( double fastVal, double slowVal )
{
	entity::PosiDirectionType retDirection = entity::NET;

	if(fastVal > slowVal)
		return entity::LONG;
	else if(fastVal < slowVal)
		return entity::SHORT;
	
	return retDirection;
}

void CWMATrendStrategy::OnBeforeAddingHistSrcConfig( CHistSourceCfg* pHistSrcCfg )
{
	if(pHistSrcCfg != NULL)
	{
		if(pHistSrcCfg->Precision == m_period)
			pHistSrcCfg->HistData = true;
	}
}

int CWMATrendStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}
