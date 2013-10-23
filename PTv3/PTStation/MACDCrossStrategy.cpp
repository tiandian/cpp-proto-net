#include "StdAfx.h"
#include "MACDCrossStrategy.h"
#include "MACDCrossTrigger.h"
#include "TrailingStopTrigger.h"
#include "Portfolio.h"
#include "PortfolioTrendOrderPlacer.h"
#include "DoubleCompare.h"
#include "PriceBarDataProxy.h"
#include "globalmembers.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"
#include "OHLCRecordSet.h"

CMACDCrossStrategy::CMACDCrossStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_macdShort(0)
	, m_macdLong(0)
	, m_macdM(0)
	, m_bollM(0)
	, m_bollP(0)
	, m_fastPeriod(0)
	, m_slowPeriod(0)
	, m_fastHistVal(0.0)
	, m_slowHistVal(0.0)
	, m_bollTop(0.0)
	, m_bollBottom(0.0)
	, m_bollMid(0.0)
	, m_pOpenTrigger(NULL)
	, m_pCloseTrigger(NULL)
	, m_pTrailingStopTrigger(NULL)
	, m_marketOpen(false)
	, m_lastBollPosition(UNKOWN_BOLL_POSITION)
	, m_openAtBarIdx(0)
{
	m_macdHistArr[0] = 0.0;
	m_macdHistArr[1] = 0.0;

	Apply(strategyItem, false);

	CreateTriggers(strategyItem);
}


CMACDCrossStrategy::~CMACDCrossStrategy(void)
{
}

void CMACDCrossStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	m_macdShort = strategyItem.hs_short();
	m_macdLong = strategyItem.hs_long();
	m_macdM = strategyItem.hs_m();
	m_bollM = strategyItem.dx_bollm();
	m_bollP = strategyItem.dx_bollp();

	m_fastPeriod = strategyItem.hs_fastperiod();
	m_slowPeriod = strategyItem.hs_slowperiod();

	if(withTriggers)
	{
		// Editing parameters of strategy, including seed of MACDDataSet
		if(m_fastPeriodIndicatorSet.get() != NULL)
		{
			if(!DoubleEqual(m_fastPeriodIndicatorSet->SeedShort(), strategyItem.hs_fastshortemaseed())
				|| !DoubleEqual(m_fastPeriodIndicatorSet->SeedLong(), strategyItem.hs_fastlongemaseed())
				|| !DoubleEqual(m_fastPeriodIndicatorSet->SeedSignal(), strategyItem.hs_fastsignalemaseed()))
			{
				m_fastPeriodIndicatorSet->SeedShort(strategyItem.hs_fastshortemaseed());
				m_fastPeriodIndicatorSet->SeedLong(strategyItem.hs_fastlongemaseed());
				m_fastPeriodIndicatorSet->SeedSignal(strategyItem.hs_fastsignalemaseed());
				m_fastPeriodIndicatorSet->ResetSeedPosition();
			}
		}

		if(m_slowPeriodIndicatorSet.get() != NULL)
		{
			if(!DoubleEqual(m_slowPeriodIndicatorSet->SeedShort(), strategyItem.hs_slowshortemaseed())
				|| !DoubleEqual(m_slowPeriodIndicatorSet->SeedLong(), strategyItem.hs_slowlongemaseed())
				|| !DoubleEqual(m_slowPeriodIndicatorSet->SeedSignal(), strategyItem.hs_slowsignalemaseed()))
			{
				m_slowPeriodIndicatorSet->SeedShort(strategyItem.hs_slowshortemaseed());
				m_slowPeriodIndicatorSet->SeedLong(strategyItem.hs_slowlongemaseed());
				m_slowPeriodIndicatorSet->SeedSignal(strategyItem.hs_slowsignalemaseed());
				m_slowPeriodIndicatorSet->ResetSeedPosition();
			}
		}
	}
	else
	{
		// don't touch hist data source when editing strategy
		PrepareHistDataSrc(strategyItem);

		// Initialize Indicator set
		const vector<CPriceBarDataProxy*>& dataProxies = DataProxies();
		for(vector<CPriceBarDataProxy*>::const_iterator iter = dataProxies.begin(); iter != dataProxies.end(); ++iter)
		{
			if((*iter)->Precision() == m_fastPeriod)
			{
				m_fastPeriodIndicatorSet = MACDDataSetPtr(new CMACDDataSet((*iter)->GetRecordSetSize(), 
					m_macdShort, m_macdLong, m_macdM));
				m_fastPeriodIndicatorSet->SeedShort(strategyItem.hs_fastshortemaseed());
				m_fastPeriodIndicatorSet->SeedLong(strategyItem.hs_fastlongemaseed());
				m_fastPeriodIndicatorSet->SeedSignal(strategyItem.hs_fastsignalemaseed());

				m_fastBollIndicatorSet = BollDataSetPtr(new CBollDataSet((*iter)->GetRecordSetSize(),
					m_bollM, m_bollP));
			}
			else if((*iter)->Precision() == m_slowPeriod)
			{
				m_slowPeriodIndicatorSet = MACDDataSetPtr(new CMACDDataSet((*iter)->GetRecordSetSize(),
					m_macdShort, m_macdLong, m_macdM));
				m_slowPeriodIndicatorSet->SeedShort(strategyItem.hs_slowshortemaseed());
				m_slowPeriodIndicatorSet->SeedLong(strategyItem.hs_slowlongemaseed());
				m_slowPeriodIndicatorSet->SeedSignal(strategyItem.hs_slowsignalemaseed());
			}
		}
	}
}

void CMACDCrossStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		const entity::TriggerItem& triggerItem = strategyItem.triggers(i);
		if(triggerItem.name() == CrossOpenTriggerName)
		{
			m_pOpenTrigger = new CMACDCrossOpenTrigger(triggerItem);
			TriggerPtr trigger(m_pOpenTrigger);
			m_triggers.push_back(trigger);
		}
		else if(triggerItem.name() == CrossCloseTriggerName)
		{
			m_pCloseTrigger = new CMACDCrossCloseTrigger(triggerItem);
			TriggerPtr trigger(m_pCloseTrigger);
			m_triggers.push_back(trigger);
		}
		else if(triggerItem.name() == CrossTrailingStopTriggerName)
		{
			m_pTrailingStopTrigger = new CTrailingStopTrigger(triggerItem);
			TriggerPtr trigger(m_pTrailingStopTrigger);
			m_triggers.push_back(trigger);
		}
	}
}

void CMACDCrossStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_hs_fastmacdhist(m_fastHistVal);
	pPortfUpdateItem->set_hs_slowmacdhist(m_slowHistVal);
	pPortfUpdateItem->set_dx_bolltop(m_bollTop);
	pPortfUpdateItem->set_dx_bollbottom(m_bollBottom);
}

void CMACDCrossStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);

	//pQuote->set_last(2428.4);
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

	COHLCRecordSet* slowOHLC = GetRecordSet(symbol, m_slowPeriod, timestamp);
	m_slowPeriodIndicatorSet->Calculate(slowOHLC);

	m_slowHistVal = m_slowPeriodIndicatorSet->GetRef(IND_MACD_HIST, 0);
	m_macdHistArr[0] = m_slowHistVal;

	COHLCRecordSet* fastOHLC = GetRecordSet(symbol, m_fastPeriod, timestamp);
	m_fastPeriodIndicatorSet->Calculate(fastOHLC);
	int currentBarIdx = fastOHLC->GetEndIndex();

	m_fastHistVal = m_fastPeriodIndicatorSet->GetRef(IND_MACD_HIST, 0);
	m_macdHistArr[1] = m_fastHistVal;
	
	m_fastBollIndicatorSet->Calculate(fastOHLC);
	m_bollTop = m_fastBollIndicatorSet->GetRef(IND_TOP, 0);
	m_bollBottom = m_fastBollIndicatorSet->GetRef(IND_BOTTOM, 0);
	m_bollMid = m_fastBollIndicatorSet->GetRef(IND_MID, 0);

	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Double Cross - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = false;
		if(currentBarIdx > m_openAtBarIdx) // This close condition check is only effective on the bar after open
		{
			meetCloseCondition = m_pCloseTrigger->Test(m_macdHistArr, 2);
			if(meetCloseCondition)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Double Cross - Portfolio(%s) Closing position due to fast MACD reverse cross")
					% pPortfolio->InvestorId() % pPortfolio->ID()));
				ClosePosition(pOrderPlacer, pQuote, "MACD快线逆向叉");
				return;
			}
		}
		
		meetCloseCondition = m_pTrailingStopTrigger->Test(pQuote->last());
		if(meetCloseCondition)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Double Cross - Portfolio(%s) Closing position due to Trailing stop")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ClosePosition(pOrderPlacer, pQuote, "回头触发止损(盈)");
			return;
		}

		return;	// don't need to go to test open trigger any more
	}
	
	entity::PosiDirectionType direction = GetDirection(m_slowHistVal, pQuote->last(), m_bollTop, m_bollMid, m_bollBottom);
	LOG_DEBUG(logger, boost::str(boost::format("[%s] Double Cross - Portfolio(%s) Testing open direction - slow hist:%.2f, last:%.2f, top:%.2f, mid:%.2f, bottom:%.2f -->> %s")
		% pPortfolio->InvestorId() % pPortfolio->ID() % m_slowHistVal % pQuote->last() % m_bollTop % m_bollMid % m_bollBottom % GetPosiDirectionText(direction)));
	if(direction > entity::NET)
	{
		if(!pOrderPlacer->IsWorking())
		{
			bool meetOpenCondition = m_pOpenTrigger->Test(m_macdHistArr, 2);
			if(meetOpenCondition)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Double Cross - Portfolio(%s) Opening position at bar %d")
					% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx ));
				OpenPosition(direction, pOrderPlacer, pQuote, timestamp);
				m_openAtBarIdx = currentBarIdx;
				return;
			}
		}
	}

}

void CMACDCrossStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp )
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

		LOG_DEBUG(logger, boost::str(boost::format("Double Cross - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));
		pOrderPlacer->SetMlOrderStatus(boost::str(boost::format("MACD快慢线同向交叉 - %s 开仓 @ %.2f")
			% GetPosiDirectionText(direction) % lmtPrice[0]));

		pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);

		if(m_pCloseTrigger != NULL)
		{
			m_pCloseTrigger->SetDirection(direction);
		}
		if(m_pTrailingStopTrigger != NULL)
		{
			m_pTrailingStopTrigger->Enable(lmtPrice[0], direction);
		}
	}
}

void CMACDCrossStrategy::ClosePosition( CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText )
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

		LOG_DEBUG(logger, boost::str(boost::format("Double Cross - %s Close position @ %.2f (%s)")
			% GetPosiDirectionText(posiDirection) % closePx  % pQuote->update_time()));

		pOrderPlacer->CloseOrder(closePx);

		if(m_pCloseTrigger != NULL)
		{
			// reset direction of close trigger
			m_pCloseTrigger->SetDirection(entity::NET);
		}
		m_openAtBarIdx = 0; // reset open bar position
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection) % closePx));

	}
}

entity::PosiDirectionType CMACDCrossStrategy::GetDirection( double slowMacdHist, double lastPx, double bollTop, double bollMid, double bollBottom )
{
	entity::PosiDirectionType retDirection = entity::NET;

	// slow MACD golden cross
	if(slowMacdHist > 0)
	{
		if(lastPx > bollTop		// boll top broken
			|| (lastPx > bollMid && m_lastBollPosition < ABOVE_MIDDLE))	// break up boll mid
		retDirection = entity::LONG;
	}
	// slow MACD dead cross 
	else if(slowMacdHist < 0)
	{
		if(lastPx < bollBottom	// boll bottom broken
			|| (lastPx < bollMid && m_lastBollPosition > BELOW_MIDDLE)) // break down boll mid
		retDirection = entity::SHORT;
	}

	if(lastPx > bollTop)
		m_lastBollPosition = ABOVE_UPPER;
	else if(lastPx >= bollMid)
		m_lastBollPosition = ABOVE_MIDDLE;
	else if(lastPx >= bollBottom)
		m_lastBollPosition = BELOW_MIDDLE;
	else if(lastPx > 0 && lastPx < bollBottom)
		m_lastBollPosition = BELOW_LOWER;
	else
		m_lastBollPosition = UNKOWN_BOLL_POSITION;

	return retDirection;
}

void CMACDCrossStrategy::OnBeforeAddingHistSrcConfig( CHistSourceCfg* pHistSrcCfg )
{
	if(pHistSrcCfg != NULL)
	{
		if(pHistSrcCfg->Precision == m_fastPeriod)
			pHistSrcCfg->HistData = true;
	}
}

int CMACDCrossStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}
