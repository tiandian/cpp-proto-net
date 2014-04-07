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
	, m_active(false)
	, m_revertOnClose(false)
{
}


CStrategyOrderCommand::~CStrategyOrderCommand(void)
{
}

double CStrategyOrderCommand::Fire( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	m_active = false;

	if(m_offset)
		return OpenPosition(pQuote, pPortfolio, timestamp);
	else
	{
		double px = ClosePosition(pQuote, pPortfolio, timestamp);
		if(m_revertOnClose)
			Revert(m_direction);
		return px;
	}
}

double CStrategyOrderCommand::OpenPosition( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	double px = -1.0;
	if(m_pOrderPlacer->IsWorking())
	{
		logger.Error(boost::str(boost::format("[%s] Range Trend - Portfolio(%s) OPEN position failed due to Order Placer has not done last job") 
			% pPortfolio->InvestorId() % pPortfolio->ID()));
		return px;
	}

	if(m_direction > entity::NET)
	{
		double lmtPrice[2];
		if(m_direction == entity::LONG)
		{
			px = lmtPrice[0] = pQuote->ask();
		}
		else if(m_direction == entity::SHORT)
		{
			px = lmtPrice[0] = pQuote->bid();
		}
		lmtPrice[1] = 0.0;

		LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) %s Open position @ %.2f (%s)")
			% pPortfolio->InvestorId() % pPortfolio->ID()
			% GetPosiDirectionText(m_direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = boost::str(boost::format("%s - %s ¿ª²Ö @ %.2f")
			% m_strNote % GetPosiDirectionText(m_direction) % lmtPrice[0]);

		m_pOrderPlacer->SetMlOrderStatus(openComment);

		m_pOrderPlacer->Run(m_direction, lmtPrice, 2, timestamp);

		//m_lastPositionOffset = direction;
		//m_isRealSignal = m_allowFakeSignal ? true : false; // When opening position, not sure current bar is real signal or not
		m_parentStrategy->ResetForceOpen();

	}

	return px;
}

double CStrategyOrderCommand::ClosePosition( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	double px = -1.0;

	if(m_pOrderPlacer != NULL)
	{
		entity::PosiDirectionType posiDirection = m_pOrderPlacer->PosiDirection();
		SetDirection(posiDirection);

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
		px = closePx;

		m_parentStrategy->ResetForceClose();
		
		pPortfolio->PushMessage(boost::str(boost::format("%s - %s Æ½²Ö @ %.2f")
			% m_strNote % GetPosiDirectionText(posiDirection, true) % closePx));
	}

	return px;
}

void CStrategyOrderCommand::Revert( entity::PosiDirectionType direction )
{
	if(m_offset == entity::CLOSE && direction > entity::NET)
	{
		m_offset = entity::OPEN;
		SetDirection( direction == entity::LONG ? entity::SHORT : entity::LONG );
	}
}
