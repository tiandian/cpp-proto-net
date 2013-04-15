#include "ScalperStrategy2.h"
#include "PortfolioOrderPlacer.h"
#include "globalmembers.h"

#include <math.h>
#include <boost/format.hpp>

CScalperStrategy2::CScalperStrategy2(CPortfolioOrderPlacer* pOrdPlacer)
	: m_pOrdPlacer(pOrdPlacer)
	,m_prevAsk(0), m_prevBid(0),
	m_Ask(0), m_AskSize(0), m_Bid(0), m_BidSize(0),
	m_priceTick(0.2), m_threshold(0.8),
	m_maxRetryTimes(8),
	m_openTimeout(200),m_isRunning(false)
{
}


CScalperStrategy2::~CScalperStrategy2(void)
{
}

void CScalperStrategy2::Test( entity::Quote* pQuote )
{
	if(!m_isRunning) return;
	
	m_Ask = pQuote->ask();
	m_AskSize= pQuote->ask_size();
	m_Bid = pQuote->bid();
	m_BidSize= pQuote->bid_size();

	double diff = m_Ask - m_Bid;

	// Don't need to test for the first time quote coming in
	if(m_AskSize > 0 && m_BidSize > 0)
	{
		LOG_DEBUG(logger, boost::str(boost::format("Diff: %.2f ?>= %.2f. Placer ready? %s") 
			% diff % m_threshold % (!(m_pOrdPlacer->IsWorking()) ? "Y" : "N")));
		if(!(m_pOrdPlacer->IsWorking()))
		{
			if(DoubleGreaterEqual(diff, m_threshold))
			{
				trade::PosiDirectionType direction = GetTradeDirection();

				logger.Info(boost::str(boost::format("[%s] Ask: %.2f => %.2f, Bid: %.2f => %.2f, Ask size VS Bid size: %d vs %d")
					% (direction > trade::NET ? (direction == trade::LONG ? "LONG" : "SHORT") : "IGNORE") 
					% m_prevAsk % m_Ask % m_prevBid % m_Bid % m_AskSize % m_BidSize));
				if(direction > trade::NET)
				{
					double lmtPrice[2];
					if(direction == trade::LONG)
					{
						lmtPrice[0] = m_Bid + m_priceTick;
						lmtPrice[1] = m_Ask - m_priceTick;
					}
					else // Sell
					{
						lmtPrice[0] = m_Ask - m_priceTick;
						lmtPrice[1] = m_Bid + m_priceTick;
					}

					m_pOrdPlacer->Run(direction, lmtPrice, 2);

				}
			}
		}
		else
		{
			m_pOrdPlacer->OnQuoteReceived(pQuote);
		}
	}

	m_prevAsk = m_Ask;
	m_prevBid = m_Bid;
}

void CScalperStrategy2::ApplySettings( const string& settingData )
{
	entity::ScalperSettings settings;
	settings.ParseFromString(settingData);

	m_threshold = settings.threshold();
	m_priceTick = settings.pricetick();
	m_CaseLE2Tick = settings.casele2tick();
	m_CaseLE3Tick = settings.casele3tick();
	m_CaseGE4Tick = settings.casege4tick();
	m_CaseNoChange = settings.casenochange();

	m_maxRetryTimes = settings.retrytimes();
	m_openTimeout = settings.opentimeout();

	LOG_DEBUG(logger, boost::str(boost::format("Applying scalper strategy settings. Threshold: %.2f")
		% settings.threshold()));
}

trade::PosiDirectionType CScalperStrategy2::GetTradeDirection()
{
	double askDiff = fabs(m_Ask - m_prevAsk);
	double bidDiff = fabs(m_Bid - m_prevBid);

	double absDiffDiff = fabs(askDiff - bidDiff);

	if(m_Ask > m_prevAsk && m_Bid > m_prevBid)
	{
		return trade::LONG;
	}
	else if(m_Ask < m_prevAsk && m_Bid < m_prevBid)
	{
		return trade::SHORT;
	}
	else if(DoubleEqual(m_Bid, m_prevBid) && DoubleEqual(m_Ask, m_prevAsk))
	{
		return CalcTradeDirection(m_AskSize, m_BidSize, askDiff, bidDiff, m_CaseNoChange);
	}
	else if(DoubleLessEqual(absDiffDiff, m_priceTick * 2))
	{
		return CalcTradeDirection(m_AskSize, m_BidSize, askDiff, bidDiff, m_CaseLE2Tick);
	}
	else if(DoubleLessEqual(absDiffDiff, m_priceTick * 3))
	{
		return CalcTradeDirection(m_AskSize, m_BidSize, askDiff, bidDiff, m_CaseLE3Tick);
	}
	else if(DoubleGreaterEqual(absDiffDiff, m_priceTick * 4))
	{
		return CalcTradeDirection(m_AskSize, m_BidSize, askDiff, bidDiff, m_CaseGE4Tick);
	}
	else
	{
		return m_AskSize < m_BidSize ? trade::SHORT : trade::LONG;
	}
}

trade::PosiDirectionType CScalperStrategy2::CalcTradeDirection(int askSize, int bidSize, double askDiff, double bidDiff, entity::DirectionDepends dependsOn)
{
	switch (dependsOn)
	{
	case entity::ON_SMALL_SIZE:
		return askSize < bidSize ? trade::SHORT : trade::LONG;
	case entity::ON_BIG_SIZE:
		return askSize > bidSize ? trade::SHORT : trade::LONG;
	case entity::ON_SMALL_CHANGE:
		return askDiff < bidDiff ? trade::SHORT : trade::LONG;
	case entity::ON_BIG_CHANGE:
		return askDiff > bidDiff ? trade::SHORT : trade::LONG;
	}

	return trade::NET;
}
