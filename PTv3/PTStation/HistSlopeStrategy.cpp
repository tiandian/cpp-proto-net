#include "StdAfx.h"
#include "HistSlopeStrategy.h"
#include "HistSlopeTrigger.h"
#include "TrailingStopTrigger.h"
#include "AvatarClient.h"
#include "OHLCRecordSet.h"
#include "PriceBarDataProxy.h"
#include "PortfolioTrendOrderPlacer.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "globalmembers.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"

#define PI 3.1415926

entity::SlopeDirection CheckDirection(double point1, double point2)
{
	if(point2 - point1 > 0.01)
		return entity::GOING_UP;
	else if(point1 - point2 > 0.01)
		return entity::GOING_DOWN;
	else
		return entity::NO_DIRECTION;
}

CHistSlopeStrategy::CHistSlopeStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_macdShort(0)
	, m_macdLong(0)
	, m_macdM(0)
	, m_fastPeriod(0)
	, m_slowPeriod(0)
	, m_fastStdDiff(0)
	, m_slowStdDiff(0)
	, m_positionOpened(false)
	, m_marketOpen(false)
	, m_fastHistVal(0.0)
	, m_fastHistDiff(0.0)
	, m_slowHistVal(0.0)
	, m_slowHistDiff(0.0)
	, m_fastSlopeDirection(entity::NO_DIRECTION)
	, m_slowSlopeDirection(entity::NO_DIRECTION)
	, m_pTrailingStopTrigger(NULL)
	, m_pOpenTrigger(NULL)
	, m_pCloseTrigger(NULL)
{
	m_angleArray[0] = 0;
	m_angleArray[1] = 0;

	Apply(strategyItem, false);

	CreateTriggers(strategyItem);
}


CHistSlopeStrategy::~CHistSlopeStrategy(void)
{
	boost::mutex::scoped_lock l(m_mut);

	m_pTrailingStopTrigger = NULL;
	m_pOpenTrigger = NULL;
	m_pCloseTrigger = NULL;
}

void CHistSlopeStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);
	
	m_macdShort = strategyItem.hs_short();
	m_macdLong = strategyItem.hs_long();
	m_macdM = strategyItem.hs_m();
	m_fastStdDiff = strategyItem.hs_faststddiff();
	m_slowStdDiff = strategyItem.hs_slowstddiff();
	m_fastPeriod = strategyItem.hs_fastperiod();
	m_slowPeriod = strategyItem.hs_slowperiod();

	if(m_openTimeout == 0)
		m_openTimeout = 350;
	if(m_retryTimes == 0)
		m_retryTimes = 8;

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

void CHistSlopeStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		const entity::TriggerItem& triggerItem = strategyItem.triggers(i);
		if(triggerItem.name() == HistSlopeTriggerName)
		{
			entity::PosiOffsetFlag posiOffset = triggerItem.hs_offset();
			CHistSlopeTrigger* pT = NULL;
			if(posiOffset == entity::OPEN)
			{
				m_pOpenTrigger = new CHistSlopeOpenTrigger(triggerItem);
				pT = m_pOpenTrigger;
			}
			else if(posiOffset == entity::CLOSE)
			{
				m_pCloseTrigger = new CHistSlopeCloseTrigger(triggerItem);
				pT = m_pCloseTrigger;
			}

			if(pT != NULL)
			{
				TriggerPtr trigger(pT);
				m_triggers.push_back(trigger);
			}
		}
		else if(triggerItem.name() == HistSlopeTrailingStopTriggerName)
		{
			m_pTrailingStopTrigger = new CTrailingStopTrigger(triggerItem);
			TriggerPtr trigger(m_pTrailingStopTrigger);
			m_triggers.push_back(trigger);
		}
	}
}

void CHistSlopeStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);
	
	//pQuote->set_last(2407.4);
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

	// 1. Calculate MACD hist value of 1 min and 5 min
	string symbol = pQuote->symbol();

	COHLCRecordSet* slowOHLC = GetRecordSet(symbol, m_slowPeriod, timestamp);
	if(slowOHLC != NULL)
		m_slowPeriodIndicatorSet->Calculate(slowOHLC);
	
	double slowLast0 = m_slowPeriodIndicatorSet->GetRef(IND_MACD_HIST, 0);
	double slowLast1 = m_slowPeriodIndicatorSet->GetRef(IND_MACD_HIST, 1);
	m_slowHistVal = slowLast0;
	// 2. Test 5 min angle, see if Up or Down.
	m_slowSlopeDirection = CheckDirection(slowLast1, slowLast0);

	// 3. Calculate value of 1 min angle
	COHLCRecordSet* fastOHLC = GetRecordSet(symbol, m_fastPeriod, timestamp);
	if(fastOHLC != NULL)
		m_fastPeriodIndicatorSet->Calculate(fastOHLC);
	// 3.1 if sign of 1 min is same as 5 min, Goes to Trigger test
	double fastLast0 = m_fastPeriodIndicatorSet->GetRef(IND_MACD_HIST, 0);
	double fastLast1 = m_fastPeriodIndicatorSet->GetRef(IND_MACD_HIST, 1);
	m_fastHistVal = fastLast0;
	m_fastSlopeDirection = CheckDirection(fastLast1, fastLast0);

	m_fastHistDiff = fastLast0 - fastLast1;
	m_angleArray[0] = CalculateAngle(m_fastStdDiff, m_fastHistDiff);
	m_slowHistDiff = slowLast0 - slowLast1;
	m_angleArray[1] = CalculateAngle(m_slowStdDiff, m_slowHistDiff);

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
		bool meetCloseCondition = m_pCloseTrigger->Test(m_angleArray, 2);
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
}

void CHistSlopeStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	
	pPortfUpdateItem->set_hs_fastangle(m_angleArray[0]);
	pPortfUpdateItem->set_hs_slowangle(m_angleArray[1]);
	pPortfUpdateItem->set_hs_fastmacdhist(m_fastHistVal);
	pPortfUpdateItem->set_hs_fastmacdhistdiff(m_fastHistDiff);
	pPortfUpdateItem->set_hs_slowmacdhist(m_slowHistVal);
	pPortfUpdateItem->set_hs_slowmacdhistdiff(m_slowHistDiff);
	pPortfUpdateItem->set_hs_fastslopedirection(m_fastSlopeDirection);
	pPortfUpdateItem->set_hs_slowslopedirection(m_slowSlopeDirection);
}

int CHistSlopeStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

double CHistSlopeStrategy::CalculateAngle(double stdHistDiff, double currentHistDiff)
{
	double tanVal = currentHistDiff / stdHistDiff;
	double arcTan = atan(tanVal);
	double angle = arcTan * 180 / PI;
	return angle;
}

void CHistSlopeStrategy::OpenPosition( entity::SlopeDirection slopeDirection, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp )
{
	if(slopeDirection > entity::NO_DIRECTION)
	{
		double lmtPrice[2];
		entity::PosiDirectionType direction = entity::LONG;
		if(slopeDirection == entity::GOING_UP)
		{
			lmtPrice[0] = pQuote->ask();
			direction = entity::LONG;
		}
		else if(slopeDirection == entity::GOING_DOWN)
		{
			lmtPrice[0] = pQuote->bid();
			direction = entity::SHORT;
		}
		lmtPrice[1] = 0.0;

		LOG_DEBUG(logger, boost::str(boost::format("HistSlope - %s Open position @ %.2f")
			% GetPosiDirectionText(direction) % lmtPrice[0] ));
		pOrderPlacer->SetMlOrderStatus(boost::str(boost::format("MACD角度满足 - %s 开仓 @ %.2f")
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

void CHistSlopeStrategy::ClosePosition( CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText )
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

		LOG_DEBUG(logger, boost::str(boost::format("HistSlope - %s Close position @ %.2f")
			% GetPosiDirectionText(posiDirection) % closePx ));
		
		pOrderPlacer->CloseOrder(closePx);

		if(m_pCloseTrigger != NULL)
		{
			// reset direction of close trigger
			m_pCloseTrigger->SetDirection(entity::NET);
		}

		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection) % closePx));

	}
}
