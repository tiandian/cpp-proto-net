#pragma once

#include "QuoteAgent.h"
#include "TradeAgent.h"
#include "StrategyManager.h"
#include "PortfolioManager.h"
#include "ManualOrderPlacer.h"

class CClientAgent
{
public:
	CClientAgent(void);
	~CClientAgent(void);

private:
	CQuoteAgent			m_quoteAgent;
	CTradeAgent			m_tradeAgent;
	CStrategyManager	m_strategyMgr;
	CPortfolioManager	m_portfolioMgr;
	CManualOrderPlacer	m_orderPlacer;
};

