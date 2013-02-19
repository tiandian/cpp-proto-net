#include "StdAfx.h"
#include "ScalperStrategy.h"

#define IF_TICK 0.2

CScalperStrategy::CScalperStrategy(void):
m_prevAsk(0), m_prevBid(0),
m_Ask(0), m_AskSize(0), m_Bid(0), m_BidSize(0) 
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

trade::PosiDirectionType CScalperStrategy::GetTradeDirection()
{
	if(m_Ask > m_prevAsk && m_Bid > m_prevBid)
	{
		return trade::LONG;
	}
	else if(m_Ask < m_prevAsk && m_Bid < m_prevBid)
	{
		return trade::SHORT;
	}
	else if(DoubleEqual(m_Bid, m_prevBid) && DoubleGreaterEqual(m_Ask - m_prevAsk, IF_TICK * 4))
	{
		return trade::LONG;
	}
	else if(DoubleEqual(m_Ask, m_prevAsk) && DoubleGreaterEqual(m_prevBid - m_Bid, IF_TICK * 4))
	{
		return trade::SHORT;
	}
	else if(DoubleEqual(m_Bid, m_prevBid) && DoubleEqual(m_Ask, m_prevAsk))
	{
		return m_AskSize > m_BidSize ? trade::SHORT : trade::LONG;
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
			% (direction == trade::LONG ? "LONG" : "SHORT") 
			% m_prevAsk % m_Ask % m_prevBid % m_Bid % m_AskSize % m_BidSize));
		Client()->QuickScalpe(portf, portf->Quantity(), direction, IF_TICK);
	}
}


