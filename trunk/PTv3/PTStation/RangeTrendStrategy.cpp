#include "StdAfx.h"
#include "RangeTrendStrategy.h"
#include "PriceBarDataProxy.h"
#include "globalmembers.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"
#include "Portfolio.h"
#include "PortfolioTrendOrderPlacer.h"
#include "OHLCRecordSet.h"
#include "DoubleCompare.h"



CRangeTrendStrategy::CRangeTrendStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_timeFrame(300)
	, m_marketOpen(false)
	, m_openPeriod(20)
	, m_closePeriod(10)
	, m_stopLossFactor(2.0)
	, m_trendFactor(2.0)
	, m_upperBoundOpen(DOUBLE_MAX_PRICE)
	, m_lowerBoundOpen(DOUBLE_MIN_PRICE)
	, m_upperBoundClose(DOUBLE_MAX_PRICE)
	, m_lowerBoundClose(DOUBLE_MIN_PRICE)
	, m_currentHigh(0.0)
	, m_currentLow(0.0)
	, m_NATR(0.0)
	, m_StopLoss(0.0)
	, m_lastBarIdx(-1)
	, m_lastPosiDirection(entity::NET)
	, m_lastCostPx(-1.0)
	, m_recentStopLossPx(0)
	, m_trending(false)
{
}


CRangeTrendStrategy::~CRangeTrendStrategy(void)
{
}

void CRangeTrendStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	m_timeFrame = strategyItem.rt_timeframe();
	m_openPeriod = strategyItem.rt_openperiod();
	m_closePeriod = strategyItem.rt_closeperiod();
	m_stopLossFactor = strategyItem.rt_stoplossfactor();
	m_trendFactor = strategyItem.rt_trendfactor();
	
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
			if((*iter)->Precision() == m_timeFrame)
			{
				m_openDonchianDataSet = DonchianDataSetPtr(new CDonchianDataSet((*iter)->GetRecordSetSize(), m_openPeriod));
				m_closeDonchianDataSet = DonchianDataSetPtr(new CDonchianDataSet((*iter)->GetRecordSetSize(), m_closePeriod));
				m_atrDataSet = ATRDataSetPtr(new CATRDataSet((*iter)->GetRecordSetSize(), m_closePeriod));
			}
		}
	}
}

void CRangeTrendStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
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

	COHLCRecordSet* ohlc = GetRecordSet(symbol, m_timeFrame, timestamp);
	if(ohlc == NULL)
		return;

	//int forceCloseBar = ohlc->GetSize() - m_forceCloseOffset;

	int currentBarIdx = ohlc->GetEndIndex();

	if(currentBarIdx <= m_openPeriod)
		return;	// too few bars to test

	m_openDonchianDataSet->Calculate(ohlc);
	m_upperBoundOpen = m_openDonchianDataSet->GetRef(IND_DONCHIAN_Hi, 1);
	m_lowerBoundOpen = m_openDonchianDataSet->GetRef(IND_DONCHIAN_Lo, 1);

	m_closeDonchianDataSet->Calculate(ohlc);
	m_upperBoundClose = m_closeDonchianDataSet->GetRef(IND_DONCHIAN_Hi, 1);
	m_lowerBoundClose = m_closeDonchianDataSet->GetRef(IND_DONCHIAN_Lo, 1);

	m_atrDataSet->Calculate(ohlc);
	m_NATR = m_atrDataSet->GetRef(IND_ATR, 1);

	if(currentBarIdx > m_lastBarIdx)
	{
		m_lastBarIdx = currentBarIdx;
		if(m_pendingOrdCmd.get() != NULL)	// has pending command
		{
			if(m_pendingOrdCmd->IsActive())
			{
				// Fire command
				if(m_pendingOrdCmd->GetOffset() == entity::OPEN)		// For Open position command
				{
					m_lastPosiDirection = m_pendingOrdCmd->GetDirection();
					m_lastCostPx = m_pendingOrdCmd->Fire(pQuote, pPortfolio, timestamp);
					m_StopLoss = m_stopLossFactor * m_NATR;
					LOG_DEBUG(logger, boost::str(boost::format("Fired StrategyOrderCommand(OPEN). LastCost: %.2f, StopLoss: %.2f")
						% m_lastCostPx % m_StopLoss));
				}
				else	// for close position command
				{
					// reset flags
					double closePx = m_pendingOrdCmd->Fire(pQuote, pPortfolio, timestamp);

					m_lastPosiDirection = entity::NET;
					m_lastCostPx = -1.0;
					m_StopLoss = 0;
				}

				if(!m_pendingOrdCmd->IsActive())		// command reverted will not be reset
					m_pendingOrdCmd.reset();
				
				return;
			}
			else
			{
				LOG_DEBUG(logger, boost::str(boost::format("StrategyOrderCommand is inactive, and removed.")));
				m_pendingOrdCmd.reset();
			}
		}
	}

	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	// Testing for Close Position
	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = false;
		bool forceClosing = IsForceClosing();
		if(forceClosing // This close condition check is only effective on the bar after open
			//|| currentBarIdx >= forceCloseBar
			)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Closing position due to Force close")
				% pPortfolio->InvestorId() % pPortfolio->ID()));

			CStrategyOrderCommand forceCloseCommand(entity::CLOSE, pOrderPlacer, this);
			forceCloseCommand.SetNote("手动平仓");
			double px = forceCloseCommand.Fire(pQuote, pPortfolio, timestamp);
			return;
		}

		// Stop Gain & Stop Loss
		string closeComment;
		bool revertOffset = false;
		bool closePosition = TestForClose(pPortfolio, pQuote, m_upperBoundClose, m_lowerBoundClose, &closeComment, &revertOffset);
		if(closePosition)
		{
			if(m_pendingOrdCmd.get() == NULL)
			{
				m_pendingOrdCmd = OrderCommandPtr(new CStrategyOrderCommand(entity::CLOSE, pOrderPlacer, this));
				m_pendingOrdCmd->SetNote(closeComment);
				m_pendingOrdCmd->SetRevertOnClose(revertOffset);
				m_pendingOrdCmd->Activate();
			}
			else
			{
				if(!m_pendingOrdCmd->IsActive())
				{
					m_pendingOrdCmd->SetNote(closeComment);
					m_pendingOrdCmd->SetRevertOnClose(revertOffset);
					m_pendingOrdCmd->Activate();
				}
			}
		}
		else
		{
			if(m_pendingOrdCmd.get() != NULL)
				m_pendingOrdCmd->Deactivate();
		}
	}
	// Testing for Open position
	else
	{
		string openComment;
		entity::PosiDirectionType direction = TestForOpen(pPortfolio, pQuote, m_upperBoundOpen, m_lowerBoundOpen, &openComment);

		if(direction > entity::NET)
		{
			if(m_pendingOrdCmd.get() == NULL)
			{
				m_pendingOrdCmd = OrderCommandPtr(new CStrategyOrderCommand(entity::OPEN, pOrderPlacer, this));
				m_pendingOrdCmd->SetDirection(direction);
				m_pendingOrdCmd->SetNote(openComment);
				m_pendingOrdCmd->Activate();
			}
			else{
				if(!m_pendingOrdCmd->IsActive())
				{
					m_pendingOrdCmd->SetDirection(direction);
					m_pendingOrdCmd->SetNote(openComment);
					m_pendingOrdCmd->Activate();
				}
			}
		}
		else
		{
			if(m_pendingOrdCmd.get() != NULL)
				m_pendingOrdCmd->Deactivate();
		}
	}
}

void CRangeTrendStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_rt_upperboundopen(m_upperBoundOpen);
	pPortfUpdateItem->set_rt_lowerboundopen(m_lowerBoundOpen);
	pPortfUpdateItem->set_rt_upperboundclose(m_upperBoundClose);
	pPortfUpdateItem->set_rt_lowerboundclose(m_lowerBoundClose);
	pPortfUpdateItem->set_rt_lastcostpx(m_lastCostPx);
	pPortfUpdateItem->set_rt_recentstoplosspx(m_recentStopLossPx);
}

int CRangeTrendStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

void CRangeTrendStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{

}

void CRangeTrendStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening, const char* noteText )
{
	/*
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

		LOG_DEBUG(logger, boost::str(boost::format("Range Trend - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = forceOpening ? 
			boost::str(boost::format("手动 %s 开仓 @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0])
			: 
		boost::str(boost::format("%s - %s 开仓 @ %.2f")
			% noteText % GetPosiDirectionText(direction) % lmtPrice[0]);

		pOrderPlacer->SetMlOrderStatus(openComment);

		pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);

		//m_lastPositionOffset = direction;
		//m_isRealSignal = m_allowFakeSignal ? true : false; // When opening position, not sure current bar is real signal or not
		ResetForceOpen();
	}
	*/
}

void CRangeTrendStrategy::ClosePosition( CPortfolio* pPortfolio, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText )
{
	/*
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

		LOG_DEBUG(logger, boost::str(boost::format("Range Trend - %s Close position @ %.2f (%s)")
			% GetPosiDirectionText(posiDirection) % closePx  % pQuote->update_time()));

		pOrderPlacer->CloseOrder(closePx);

		ResetForceClose();
		// reset initStopPx
		//m_initStopPx = -1.0;

		pPortfolio->PushMessage(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection, true) % closePx));
	}
	*/
}

void CRangeTrendStrategy::OnBeforeAddingHistSrcConfig( CHistSourceCfg* pHistSrcCfg )
{
	if(pHistSrcCfg != NULL)
	{
		if(pHistSrcCfg->Precision == m_timeFrame)
			pHistSrcCfg->HistData = true;
	}
}

entity::PosiDirectionType CRangeTrendStrategy::TestForOpen( CPortfolio* pPortfolio, entity::Quote* pQuote, double upperBound, double lowerBound, string* pOutComment  )
{
	entity::PosiDirectionType direction = entity::NET;
	double last = pQuote->last();
	if(last > upperBound )
		direction = entity::SHORT;
	else if(last < upperBound)
		direction = entity::LONG;

	LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Testing for OPEN - Last: %.2f, UpperBound: %.2f, LowerBound: %.2f --->>> %s")
		% pPortfolio->InvestorId() % pPortfolio->ID() % last % m_upperBoundOpen % m_lowerBoundOpen % GetPosiDirectionText(direction)));
	if(direction == entity::SHORT)
	{
		*pOutComment = boost::str(boost::format("价格(%.2f)突破上轨(%.2f)空头开仓") % last % upperBound );
	}
	else if(direction == entity::LONG)
	{
		*pOutComment = boost::str(boost::format("价格(%.2f)突破下轨(%.2f)多头开仓") % last % lowerBound );
	}
	return direction;
}

bool CRangeTrendStrategy::TestForClose( CPortfolio* pPortfolio, entity::Quote* pQuote, double upperBound, double lowerBound, string* pOutComment, bool* outRevertOffset )
{
	double last = pQuote->last();
	bool ret = false;
	if(m_trending)
	{
		if(m_lastPosiDirection == entity::LONG)
		{
			double stopLossPt = m_lastCostPx - m_StopLoss;
			m_recentStopLossPx = stopLossPt;
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Testing for CLOSE (TREND-LONG) - Last: %.2f, upperBound: %.2f, stopLossPt: %.2f")
				% pPortfolio->InvestorId() % pPortfolio->ID() % last % lowerBound % stopLossPt));
			
			if(last < lowerBound || last < stopLossPt)
			{
				ret = true;
				*pOutComment = boost::str(boost::format("价格(%.2f)小于止损点(%.2f, %.2f)") % last % lowerBound % stopLossPt );
			}
		}
		else if(m_lastPosiDirection == entity::SHORT)
		{
			double stopLossPt = m_lastCostPx + m_StopLoss;
			m_recentStopLossPx = stopLossPt;
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Testing for CLOSE (TREND-SHORT) - Last: %.2f, upperBound: %.2f, stopLossPt: %.2f")
				% pPortfolio->InvestorId() % pPortfolio->ID() % last % upperBound % stopLossPt));

			if(last > upperBound || last > stopLossPt)
			{
				ret = true;
				*pOutComment = boost::str(boost::format("价格(%.2f)大于止损点(%.2f, %.2f)") % last % upperBound % stopLossPt );
			}
		}
	}
	else
	{
		if(m_lastPosiDirection == entity::LONG)
		{
			double stopGain = m_lastCostPx + m_StopLoss;
			double stopLoss = m_lastCostPx - (m_trendFactor * m_StopLoss);
			m_recentStopLossPx = stopLoss;
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Testing for CLOSE (RANGE-LONG) - Last: %.2f, StopLoss: %.2f, StopGain: %.2f")
				% pPortfolio->InvestorId() % pPortfolio->ID() % last % stopLoss % stopGain));

			if(last > stopGain)
			{
				ret = true;
				*pOutComment = boost::str(boost::format("价格(%.2f)大于止盈点(%.2f)") % last % stopGain );
			}
			else if(last < stopLoss)
			{
				ret = true;
				*outRevertOffset = true;
				*pOutComment = boost::str(boost::format("价格(%.2f)小于止损点(%.2f)") % last % stopLoss );
			}
		}
		else if(m_lastPosiDirection == entity::SHORT)
		{
			double stopGain = m_lastCostPx - m_StopLoss;
			double stopLoss = m_lastCostPx + (m_trendFactor * m_StopLoss);
			m_recentStopLossPx = stopLoss;
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Testing for CLOSE (RANGE-SHORT) - Last: %.2f, StopLoss: %.2f, StopGain: %.2f")
				% pPortfolio->InvestorId() % pPortfolio->ID() % last % stopLoss % stopGain));

			if(last < stopGain)
			{
				ret = true;
				*pOutComment = boost::str(boost::format("价格(%.2f)小于止盈点(%.2f)") % last % stopGain );
			}
			else if(last > stopLoss)
			{
				ret = true;
				*outRevertOffset = true;
				*pOutComment = boost::str(boost::format("价格(%.2f)大于止损点(%.2f)") % last % stopLoss );
			}
		}
	}
	return ret;
}
