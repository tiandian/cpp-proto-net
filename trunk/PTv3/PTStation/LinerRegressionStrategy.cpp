#include "StdAfx.h"
#include "LinerRegressionStrategy.h"
#include "globalmembers.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"
#include "PriceBarDataProxy.h"
#include "OHLCRecordSet.h"
#include "PortfolioTrendOrderPlacer.h"
#include "Portfolio.h"

CLinerRegressionStrategy::CLinerRegressionStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_period(0)
	, m_number(0)
	, m_openThreshold(90.0)
	, m_closeThreshold(90.0)
	, m_marketOpen(false)
	, m_direction(entity::NET)
	, m_openAtBarIdx(0)
{
}


CLinerRegressionStrategy::~CLinerRegressionStrategy(void)
{
}

void CLinerRegressionStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	m_period = strategyItem.lr_period();
	m_number = strategyItem.lr_number();
	m_openThreshold = strategyItem.lr_openthreshold();
	m_closeThreshold = strategyItem.lr_closethreshold();

	if(withTriggers)
	{
		if(m_linerRegDataSet.get() != NULL)
		{
			m_linerRegDataSet->SetPeriod(m_number);
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
			if((*iter)->Precision() == m_period)
			{
				m_linerRegDataSet = LinerRegSetPtr(new CLinerRegressionDataSet((*iter)->GetRecordSetSize(), m_number));
			}
		}
	}
}

void CLinerRegressionStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_lr_weightmidpoint(m_weightMidPoint);
	pPortfUpdateItem->set_lr_linerregangle(m_linerRegAngle);
}

void CLinerRegressionStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	if(!IsRunning())
		return;

	string symbol = pQuote->symbol();

	if(!m_marketOpen)
	{
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

	COHLCRecordSet* pOHLC = GetRecordSet(symbol, m_period, timestamp);
	assert(pOHLC != NULL);

	int currentBarIdx = pOHLC->GetEndIndex();
	m_weightMidPoint = (pOHLC->WeightAvgSeries)[currentBarIdx];

	m_linerRegDataSet->Calculate(pOHLC);
	m_linerRegAngle = m_linerRegDataSet->GetRef(IND_LREG, 0);

	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Liner Reg - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	if (pOrderPlacer->IsOpened())
	{
		if(currentBarIdx > m_openAtBarIdx) // This close condition check is only effective on the bar after open
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Liner Regression - Portfolio(%s) Testing close direction - %.2f ?< %.2f")
				% pPortfolio->InvestorId() % pPortfolio->ID() % m_linerRegAngle % m_closeThreshold));

			bool meetCloseCondition = m_direction == entity::SHORT ? 
				m_linerRegAngle > -m_closeThreshold : m_linerRegAngle < m_closeThreshold;

			if(meetCloseCondition)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Liner Regression - Portfolio(%s) Closing position due to Regression Angle less then close threshold")
					% pPortfolio->InvestorId() % pPortfolio->ID()));
				ClosePosition(pOrderPlacer, pQuote);
				return;
			}
		}

		return;	// don't need to go to test open trigger any more
	}

	LOG_DEBUG(logger, boost::str(boost::format("[%s] Liner Regression - Portfolio(%s) Testing open direction - %.2f ?> %.2f")
		% pPortfolio->InvestorId() % pPortfolio->ID() % m_linerRegAngle % m_openThreshold));

	if(!pOrderPlacer->IsWorking())
	{
		bool meetOpenCondition = fabs(m_linerRegAngle)  > m_openThreshold;
		if(meetOpenCondition)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Liner Regression - Portfolio(%s) Opening position at %d")
				% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx ));
			m_direction = GetDirection(m_linerRegAngle);
			OpenPosition(m_direction, pOrderPlacer, pQuote, timestamp);
			m_openAtBarIdx = currentBarIdx;
			return;
		}
	}
}

int CLinerRegressionStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

void CLinerRegressionStrategy::OnBeforeAddingHistSrcConfig( CHistSourceCfg* pHistSrcCfg )
{
	if(pHistSrcCfg != NULL)
	{
		if(pHistSrcCfg->Precision == m_period)
			pHistSrcCfg->WeightAvg = true;
	}
}

entity::PosiDirectionType CLinerRegressionStrategy::GetDirection( double angle )
{
	return angle < 0 ? entity::SHORT : entity::LONG;
}

void CLinerRegressionStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp )
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

		LOG_DEBUG(logger, boost::str(boost::format("Liner Regression - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));
		pOrderPlacer->SetMlOrderStatus(boost::str(boost::format("线性回归角度大于%.1f - %s 开仓 @ %.2f")
			% m_openThreshold % GetPosiDirectionText(direction) % lmtPrice[0]));

		pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);
	}
}

void CLinerRegressionStrategy::ClosePosition( CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote)
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

		LOG_DEBUG(logger, boost::str(boost::format("Liner Regression - %s Close position @ %.2f (%s)")
			% GetPosiDirectionText(posiDirection) % closePx  % pQuote->update_time()));

		pOrderPlacer->CloseOrder(closePx);

		m_openAtBarIdx = 0; // reset open bar position
		m_direction = entity::NET;

		pOrderPlacer->OutputStatus(boost::str(boost::format("线性回归角度小于%.2f - %s 平仓 @ %.2f")
			% m_closeThreshold % GetPosiDirectionText(posiDirection) % closePx));

	}
}
