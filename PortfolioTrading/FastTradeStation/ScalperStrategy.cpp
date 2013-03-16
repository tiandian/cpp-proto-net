#include "ScalperStrategy.h"
#include <math.h>

#define IF_TICK 0.2

CScalperStrategy::CScalperStrategy(void):
m_prevAsk(0), m_prevBid(0),
m_Ask(0), m_AskSize(0), m_Bid(0), m_BidSize(0),
m_priceTick(0)
{
}


CScalperStrategy::~CScalperStrategy(void)
{
}

void CScalperStrategy::ApplySettings( const std::string& settingData )
{
	entity::ScalperSettings settings;
	settings.ParseFromString(settingData);

	SetOpenPosiCond(GREATER_EQUAL_THAN, settings.threshold());
	m_priceTick = settings.pricetick();
	m_CaseLE2Tick = settings.casele2tick();
	m_CaseLE3Tick = settings.casele3tick();
	m_CaseGE4Tick = settings.casege4tick();
	m_CaseNoChange = settings.casenochange();
	m_closeStrategy = settings.stoplossstrategy();

	logger.Info(boost::str(boost::format("Scalper Threshold: %f") % settings.threshold()));
}

POSI_OPER CScalperStrategy::NextOperation( POSI_OPER oper )
{
	switch (oper)
	{
	case OPEN_POSI:
		return DO_NOTHING;
	default:
		return DO_NOTHING;
	}
}

void CScalperStrategy::Test()
{
	if(!m_isRunning) return;

	double diff = Portfolio()->Difference();

	m_Ask = Portfolio()->LongDiff();
	m_AskSize= Portfolio()->LongSize();
	m_Bid = Portfolio()->ShortDiff();
	m_BidSize= Portfolio()->ShortSize();
	
	// Don't need to test for the first time quote coming in
	if(m_AskSize > 0 && m_BidSize > 0)
		CStrategy<double>::Test(diff, OPEN_POSI);

	m_prevAsk = m_Ask;
	m_prevBid = m_Bid;
}

trade::PosiDirectionType CalcTradeDirection(int askSize, int bidSize, double askDiff, double bidDiff, entity::DirectionDepends dependsOn)
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

trade::PosiDirectionType CScalperStrategy::GetTradeDirection()
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

void CScalperStrategy::DoOpenPostion()
{
	logger.Info(boost::str(boost::format("Portf (%s) OPEN & CLOSE position by strategy") % Portfolio()->ID().c_str()));
	CPortfolio *portf = Portfolio();
	if(portf != NULL)
	{
		trade::PosiDirectionType direction = GetTradeDirection();
		
		logger.Info(boost::str(boost::format("[%s] Ask: %.2f => %.2f, Bid: %.2f => %.2f, Ask size VS Bid size: %d vs %d")
			% (direction > trade::NET ? (direction == trade::LONG ? "LONG" : "SHORT") : "IGNORE") 
			% m_prevAsk % m_Ask % m_prevBid % m_Bid % m_AskSize % m_BidSize));
		if(direction > trade::NET)
		{
			Client()->QuickScalpe(portf, portf->Quantity(), direction, m_priceTick);
		}
	}
}


