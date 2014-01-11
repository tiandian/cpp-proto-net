#include "StdAfx.h"
#include "ASCTrendStrategy.h"
#include "PriceBarDataProxy.h"
#include "globalmembers.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"
#include "Portfolio.h"
#include "PortfolioTrendOrderPlacer.h"
#include "OHLCRecordSet.h"

CASCTrendStrategy::CASCTrendStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_period(0)
	, m_riskParam(0)
	, m_avgPeriodParam(0)
	, m_boLengthParam(0)
	//, m_pAscStopTrigger(NULL)
	, m_marketOpen(false)
	, m_lastPositionOffset(entity::NET)
	, m_isRealSignal(false)
	, m_lastOpenBarIdx(0)
	, m_lastCloseBarIdx(-1)
	, m_williamsR(-1.0)
	, m_initStopPx(-1.0)
	, m_watr(0)
	, m_stopPx(0)
	, m_donchianHi(0)
	, m_donchianLo(0)
	, m_PDI(0)
	, m_MDI(0)
	, m_DI_Threshold(25)
	, m_X1(100)
	, m_X2(0)
	, m_costPx(0)
	, m_waitTrendEnd(false)
	, m_forceCloseOffset(5)
{

	Apply(strategyItem, false);
	CreateTriggers(strategyItem);
}


CASCTrendStrategy::~CASCTrendStrategy(void)
{
}

void CASCTrendStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	m_period = strategyItem.as_period();
	SetRisk(strategyItem.as_risk());
	m_avgPeriodParam = strategyItem.as_avergateperiod();
	m_boLengthParam = strategyItem.as_breakoutlength();

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
		for(vector<CPriceBarDataProxy*>::const_iterator iter = dataProxies.begin(); iter != dataProxies.end(); ++iter)
		{
			if((*iter)->Precision() == m_period)
			{
				m_willRIndicatorSet = WillRDataSetPtr(new CWillRDataSet((*iter)->GetRecordSetSize(), m_riskParam, m_avgPeriodParam, m_boLengthParam));
				m_watrStopIndSet = WATRStopDataSetPtr(new CWATRStopDataSet((*iter)->GetRecordSetSize(), m_avgPeriodParam));
			}
		}
	}
}

void CASCTrendStrategy::OnBeforeAddingHistSrcConfig( CHistSourceCfg* pHistSrcCfg )
{
	if(pHistSrcCfg != NULL)
	{
		if(pHistSrcCfg->Precision == m_period)
			pHistSrcCfg->HistData = true;
	}
}

void CASCTrendStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);

	//pQuote->set_last(2466);
	//pQuote->set_update_time("09:15:12");

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

	COHLCRecordSet* ohlc = GetRecordSet(symbol, m_period, timestamp);
	if(ohlc == NULL)
		return;

	int forceCloseBar = ohlc->GetSize() - m_forceCloseOffset;

	m_willRIndicatorSet->Calculate(ohlc);
	m_williamsR = m_willRIndicatorSet->GetRef(IND_WR, 0);
	m_donchianHi = m_willRIndicatorSet->GetRef(IND_Donchian_Hi, 0);
	m_donchianLo = m_willRIndicatorSet->GetRef(IND_Donchian_Lo, 0);
	//m_PDI = m_willRIndicatorSet->GetRef(IND_PDI, 0);
	//m_MDI = m_willRIndicatorSet->GetRef(IND_MDI, 0);

	m_watrStopIndSet->Calculate(ohlc);
	double trend = m_watrStopIndSet->GetRef(IND_WATR_TREND, 0);
	
	int currentBarIdx = ohlc->GetEndIndex();
	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	if (pOrderPlacer->IsOpened() || m_waitTrendEnd)
	{
		bool meetCloseCondition = false;
		bool forceClosing = IsForceClosing();
		if(forceClosing // This close condition check is only effective on the bar after open
			|| currentBarIdx >= forceCloseBar)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Closing position due to Force close")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ClosePosition(pOrderPlacer, pQuote, "手动平仓");
			return;
		}

		// check if the open bar is fake signal on next bar
		if(currentBarIdx == m_lastOpenBarIdx + 1 && !m_isRealSignal)
		{
			double preWR = m_willRIndicatorSet->GetRef(IND_WR, 1);
			m_isRealSignal = IsPreBarOpenCorrect(m_lastPositionOffset, preWR);
			LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Check if pre bar open at fake signal - preWR: %.2f (%s)")
				% pPortfolio->InvestorId() % pPortfolio->ID() % preWR % GetPosiDirectionText(m_lastPositionOffset)));
			if(!m_isRealSignal)
			{
				ClosePosition(pOrderPlacer, pQuote, "假信号开仓立即平仓");
				return;
			}
		}

		unsigned int hour = 0, min = 0, sec = -1;
		if(ParseTimeString(pQuote->update_time(), &hour, &min, &sec))
			//&& currentBarIdx > m_lastOpenBarIdx)
		{
			m_watr = m_watrStopIndSet->GetRef(IND_WATR_VAL, 0);
			m_stopPx = m_watrStopIndSet->GetRef(IND_WATR_STOP, 1);

			LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Test for closing %s position - WATR: %.2f, StopPx: %.2f, Last: %.2f")
				% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(m_lastPositionOffset) % m_watr % m_stopPx % pQuote->last()));

			bool meetCloseCondition = false;
			if(currentBarIdx > m_lastOpenBarIdx)
			{
				bool oscillatorStopLoss = OscillatorStopLoss(m_lastPositionOffset, m_costPx, m_stopPx);
				if(oscillatorStopLoss)
				{
					m_waitTrendEnd = true;	// if Sideways stop loss, don't re-open position until this trend ends
					
					LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Sideway stop loss due to trend began. CostPx: %.2f, StopPx: %.2f")
						% pPortfolio->InvestorId() % pPortfolio->ID() % m_costPx % m_stopPx));
					ClosePosition(pOrderPlacer, pQuote, "趋势形成StopPx突破成本");
					m_lastCloseBarIdx = currentBarIdx;

					return;
				}
				else
				{
					// Test Open or Close price of the bar
					if(sec <= 1 || sec >= 58)
					{
						meetCloseCondition = TestForClose(m_lastPositionOffset, pQuote->last(), m_stopPx, 0.0);
					}
					else // within bar 
					{
						meetCloseCondition = TestForClose(m_lastPositionOffset, pQuote->last(), m_stopPx, m_watr);
					}
				}
			}
			else // still the bar opening the position
			{
				double initStopPx = m_stopPx;
				double lastTrend = m_watrStopIndSet->GetRef(IND_WATR_TREND, 1);
				// in case opened position is different than trend of last bar use current bar's stopPx
				if((m_lastPositionOffset == entity::SHORT && lastTrend < 0) 
					|| (m_lastPositionOffset == entity::LONG && lastTrend > 0))
				{
					if(m_initStopPx > 0)	// when m_initStopPx was given proper value
					{
						LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Using init stopPx : %.3f")
							% pPortfolio->InvestorId() % pPortfolio->ID() % m_initStopPx));						
						initStopPx = m_initStopPx;
					}
				}
				
				meetCloseCondition = TestForClose(m_lastPositionOffset, pQuote->last(), initStopPx, 0.0);
			}

			if(meetCloseCondition)
			{
				if(m_waitTrendEnd)
				{
					LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Trend End. Ready for next sideways open")
						% pPortfolio->InvestorId() % pPortfolio->ID()));
					m_waitTrendEnd = false;
				}
				else
				{
					LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Closing position due to WATR stop")
						% pPortfolio->InvestorId() % pPortfolio->ID()));
					ClosePosition(pOrderPlacer, pQuote, "触发WATR止盈(损)");
					m_lastCloseBarIdx = currentBarIdx;
				}
				
				return;
			}
		}		

		return;	// don't need to go to test open trigger any more
	}
	
	// Testing for Open position
	double last = pQuote->last();
	LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Testing for OPEN - Last: %.2f, WR: %.2f, Hi: %.2f, Lo: %.2f, PDI: %.2f, MDI: %.2f")
		% pPortfolio->InvestorId() % pPortfolio->ID() 
		% last % m_williamsR % m_donchianHi % m_donchianLo % m_PDI % m_MDI));

	entity::PosiDirectionType direction = TestForOpen(last, m_williamsR, m_donchianHi, m_donchianLo, trend);
	if(currentBarIdx < forceCloseBar &&
		direction > entity::NET && 
		(currentBarIdx > m_lastCloseBarIdx))		// In general, don't open position at the bar just closing position
		//|| direction != m_lastPositionOffset))	// unless the direction is different
	{
		if(!pOrderPlacer->IsWorking())
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Opening position at bar %d")
				% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx ));
			OpenPosition(direction, pOrderPlacer, pQuote, timestamp, false, 
				boost::str(boost::format("WR(%.2f)满足条件") % m_williamsR).c_str());
			m_lastOpenBarIdx = currentBarIdx;
			
			// The stopPx at the current bar in this moment will be initial stop price as backup stop price.
			// in case the preceding trend is different than this one, this initial stop price will be effective.
			m_initStopPx = m_watrStopIndSet->GetRef(IND_WATR_STOP, 0);
			return;
		}
	}
}

void CASCTrendStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_as_williamsr(m_williamsR);
	pPortfUpdateItem->set_as_stoppx(m_stopPx);
	pPortfUpdateItem->set_as_donchianhi(m_donchianHi);
	pPortfUpdateItem->set_as_donchianlo(m_donchianLo);
}

int CASCTrendStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

void CASCTrendStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	/*
	entity::TriggerItem triggerItem;
	triggerItem.set_strategy(entity::ASC_TREND);
	triggerItem.set_name("WATR Stop trigger");
	triggerItem.set_enabled(false);

	m_pAscStopTrigger = new CASCTrendTrigger(triggerItem);
	TriggerPtr trigger(m_pAscStopTrigger);
	m_triggers.push_back(trigger);
	*/
}

void CASCTrendStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening, const char* noteText )
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

		LOG_DEBUG(logger, boost::str(boost::format("ASC Trend - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = forceOpening ? 
			boost::str(boost::format("手动 %s 开仓 @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0])
			: 
		boost::str(boost::format("%s - %s 开仓 @ %.2f")
			% noteText % GetPosiDirectionText(direction) % lmtPrice[0]);

		pOrderPlacer->SetMlOrderStatus(openComment);

		pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);

		m_lastPositionOffset = direction;
		m_isRealSignal = false; // When opening position, not sure current bar is real signal or not
		m_costPx = lmtPrice[0];
		ResetForceOpen();

	}
}

void CASCTrendStrategy::ClosePosition( CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText )
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

		LOG_DEBUG(logger, boost::str(boost::format("ASC Trend - %s Close position @ %.2f (%s)")
			% GetPosiDirectionText(posiDirection) % closePx  % pQuote->update_time()));

		pOrderPlacer->CloseOrder(closePx);

		ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection, true) % closePx));

		// reset initStopPx
		m_initStopPx = -1.0;
		// reset costPx
		m_costPx = 0;
	}
}

entity::PosiDirectionType CASCTrendStrategy::TestForOpen( double last, double wr, double hi, double lo, double trend)
{
	if(last > hi && wr > m_X1 && trend > 0)
		//&& PDI > MDI && PDI > m_DI_Threshold)
		return entity::SHORT;

	if(last < lo && wr < m_X2 && wr > -0.1 && trend < 0)
		//&& MDI > PDI && MDI > m_DI_Threshold)
		return entity::LONG;

	return entity::NET;
}

void CASCTrendStrategy::SetRisk( int risk )
{
	m_riskParam = risk;
	m_X1 = ASC_X1 + risk;
	m_X2 = ASC_X2 - risk;
}

bool CASCTrendStrategy::IsPreBarOpenCorrect( entity::PosiDirectionType direction, double preWR )
{
	if(direction == entity::SHORT)
	{
		return preWR > m_X1;
	}

	if(direction == entity::LONG)
	{
		return preWR < m_X2;
	}

	// something wrong, ignore
	return true;
}

bool CASCTrendStrategy::TestForClose( entity::PosiDirectionType direction, double price, double stopPx, double extraWatr )
{
	if(direction == entity::SHORT)
	{
		return price < stopPx - extraWatr;
	}
	else if(direction == entity::LONG)
	{
		return price > stopPx + extraWatr;
	}

	return false;
}

bool CASCTrendStrategy::OscillatorStopLoss( entity::PosiDirectionType direction, double cost, double stopPx )
{
	if(cost > 0 && stopPx > 0)	// see if cost is valid
	{
		if(direction == entity::LONG)
		{
			return stopPx <= cost 
				&& cost > 0; 
		}
		else if(direction == entity::SHORT)
		{
			return stopPx >= cost;
		}
	}

	return false;
}

