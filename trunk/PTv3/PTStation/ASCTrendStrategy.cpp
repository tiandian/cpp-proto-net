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
	, m_DirectionOpened(entity::NET)
	, m_pAscStopTrigger(NULL)
	, m_marketOpen(false)
	, m_openAtBarIdx(0)
	, m_williamsR(0)
	, m_watrStopVal(0)
	, m_donchianHi(0)
	, m_donchianLo(0)
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
	m_riskParam = strategyItem.as_risk();
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

	m_willRIndicatorSet->Calculate(ohlc);
	double wr = m_willRIndicatorSet->GetRef(IND_WR, 0);
	double donchianHi = m_willRIndicatorSet->GetRef(IND_Donchian_Hi, 0);
	double donchianLo = m_willRIndicatorSet->GetRef(IND_Donchian_Lo, 0);
	double watr = m_willRIndicatorSet->GetRef(IND_WATR, 0);

	int currentBarIdx = ohlc->GetEndIndex();
	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = false;
		bool forceClosing = IsForceClosing();
		if(forceClosing) // This close condition check is only effective on the bar after open
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Closing position due to Force close")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ClosePosition(pOrderPlacer, pQuote, "手动平仓");
			return;
		}

		if(m_pAscStopTrigger != NULL)
		{
			double arrArgs[2] = { pQuote->last(), watr };
			meetCloseCondition = m_pAscStopTrigger->Test(arrArgs, 2);
			if(meetCloseCondition)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Closing position due to WATR stop")
					% pPortfolio->InvestorId() % pPortfolio->ID()));
				ClosePosition(pOrderPlacer, pQuote, "触发WATR止损(盈)");
				return;
			}
		}		

		return;	// don't need to go to test open trigger any more
	}
	
	// Testing for Open position
	double last = pQuote->last();
	LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Testing for OPEN - Last: %.2f, WR: %.2f, Hi: %.2f, Lo: %.2f")
		% pPortfolio->InvestorId() % pPortfolio->ID() 
		% last % wr % donchianHi % donchianLo));

	entity::PosiDirectionType direction = TestForOpen(last, wr, donchianHi, donchianLo);
	if(direction > entity::NET)
	{
		if(!pOrderPlacer->IsWorking())
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Opening position at bar %d")
				% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx ));
			OpenPosition(direction, pOrderPlacer, pQuote, timestamp, false, 
				boost::str(boost::format("WR(%.2f)满足条件")).c_str());
			m_openAtBarIdx = currentBarIdx;
			return;
		}
	}
}

void CASCTrendStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_as_williamsr(m_williamsR);
	pPortfUpdateItem->set_as_stoppx(m_watrStopVal);
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
	entity::TriggerItem triggerItem;
	triggerItem.set_strategy(entity::ASC_TREND);
	triggerItem.set_name("WATR Stop trigger");
	triggerItem.set_enabled(false);

	m_pAscStopTrigger = new CASCTrendTrigger(triggerItem);
	TriggerPtr trigger(m_pAscStopTrigger);
	m_triggers.push_back(trigger);
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

		m_DirectionOpened = direction;
		ResetForceOpen();
		
		if(m_pAscStopTrigger != NULL)
		{
			m_pAscStopTrigger->Enable(lmtPrice[0], direction);
		}
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

		m_openAtBarIdx = 0; // reset open bar position
		ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection, true) % closePx));

	}
}

entity::PosiDirectionType CASCTrendStrategy::TestForOpen( double last, double wr, double hi, double lo )
{
	if(last > hi && wr > ASC_X1)
		return entity::LONG;

	if(last < lo && wr < ASC_X2)
		return entity::SHORT;

	return entity::NET;
}

