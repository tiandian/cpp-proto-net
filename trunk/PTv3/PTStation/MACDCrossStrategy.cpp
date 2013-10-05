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
	, m_pOpenTrigger(NULL)
	, m_pCloseTrigger(NULL)
	, m_pTrailingStopTrigger(NULL)
	, m_marketOpen(false)
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

	m_fastHistVal = m_fastPeriodIndicatorSet->GetRef(IND_MACD_HIST, 0);
	m_macdHistArr[1] = m_fastHistVal;
	
	m_fastBollIndicatorSet->Calculate(fastOHLC);
	m_bollTop = m_fastBollIndicatorSet->GetRef(IND_TOP, 0);
	m_bollBottom = m_fastBollIndicatorSet->GetRef(IND_BOTTOM, 0);

	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	// 3.3 if is closing order, pass new quote to modify close order
	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] HistSlope - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	// 3.2.2 if already opened position, test close trigger
	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = m_pCloseTrigger->Test(m_macdHistArr, 2);
		if(meetCloseCondition)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] HistSlope - Portfolio(%s) Closing position due to Angle mismaching condition")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ClosePosition(pOrderPlacer, pQuote, "MACD快线角度反向并达到设定值");
			return;
		}

		meetCloseCondition = m_pTrailingStopTrigger->Test(pQuote->last());
		if(meetCloseCondition)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] HistSlope - Portfolio(%s) Closing position due to Trailing stop")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ClosePosition(pOrderPlacer, pQuote, "回头触发止损(盈)");
			return;
		}

		return;	// don't need to go to test open trigger any more
	}
/*
	if(m_slowSlopeDirection > entity::NO_DIRECTION && m_fastSlopeDirection == m_slowSlopeDirection )
	{
		// 3.2 In scope of Trigger test
		// 3.2.1 if not opened, test open trigger
		if(!pOrderPlacer->IsWorking())
		{
			bool meetOpenCondition = m_pOpenTrigger->Test(m_angleArray, 2);
			if(meetOpenCondition)
			{
				// 3.2.1.1 Do OPEN position
				// 3.2.1.2 if open trigger is fired, be sure to enable trailing stop trigger with Enable(cost, direction)
				LOG_DEBUG(logger, boost::str(boost::format("[%s] HistSlope - Portfolio(%s) Opening position")
					% pPortfolio->InvestorId() % pPortfolio->ID()));
				OpenPosition(m_fastSlopeDirection, pOrderPlacer, pQuote, timestamp);
				return;
			}
		}
	}
	*/
}

void CMACDCrossStrategy::OpenPosition( entity::SlopeDirection slopeDirection, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp )
{

}

void CMACDCrossStrategy::ClosePosition( CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText )
{

}
