#include "StdAfx.h"
#include "StrategyOrderCommand.h"
#include "PortfolioTrendOrderPlacer.h"
#include "TechAnalyStrategy.h"
#include "Portfolio.h"
#include "globalmembers.h"


CStrategyOrderCommand::CStrategyOrderCommand(entity::PosiOffsetFlag offset, CPortfolioTrendOrderPlacer* pOrderPlacer, CTechAnalyStrategy* parentStrategy)
	: m_offset(offset)
	, m_pOrderPlacer(pOrderPlacer)
	, m_parentStrategy(parentStrategy)
	, m_forceFire(false)
{
}


CStrategyOrderCommand::~CStrategyOrderCommand(void)
{
}

void CStrategyOrderCommand::Fire( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	if(m_offset)
		OpenPosition(pQuote, pPortfolio, timestamp);
	else
		ClosePosition(pQuote, pPortfolio, timestamp);
}

void CStrategyOrderCommand::OpenPosition( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	if(m_direction > entity::NET)
	{
		double lmtPrice[2];
		if(m_direction == entity::LONG)
		{
			lmtPrice[0] = pQuote->ask();
		}
		else if(m_direction == entity::SHORT)
		{
			lmtPrice[0] = pQuote->bid();
		}
		lmtPrice[1] = 0.0;

		LOG_DEBUG(logger, boost::str(boost::format("Range Trend - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(m_direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = m_forceFire ? 
			boost::str(boost::format("手动 %s 开仓 @ %.2f") % GetPosiDirectionText(m_direction) % lmtPrice[0])
			: 
		boost::str(boost::format("%s - %s 开仓 @ %.2f")
			% m_strNote % GetPosiDirectionText(m_direction) % lmtPrice[0]);

		m_pOrderPlacer->SetMlOrderStatus(openComment);

		m_pOrderPlacer->Run(m_direction, lmtPrice, 2, timestamp);

		//m_lastPositionOffset = direction;
		//m_isRealSignal = m_allowFakeSignal ? true : false; // When opening position, not sure current bar is real signal or not
		m_parentStrategy->ResetForceOpen();

	}
}

void CStrategyOrderCommand::ClosePosition( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	if(m_pOrderPlacer != NULL)
	{
		entity::PosiDirectionType posiDirection = m_pOrderPlacer->PosiDirection();

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

		m_pOrderPlacer->CloseOrder(closePx);

		m_parentStrategy->ResetForceClose();
		
		pPortfolio->PushMessage(boost::str(boost::format("%s - %s 平仓 @ %.2f")
			% m_strNote % GetPosiDirectionText(posiDirection, true) % closePx));
	}
}
