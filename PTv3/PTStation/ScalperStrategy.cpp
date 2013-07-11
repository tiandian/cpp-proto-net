#include "StdAfx.h"
#include "ScalperStrategy.h"
#include "ScalperTrigger.h"
#include "Portfolio.h"

CScalperStrategy::CScalperStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
	, m_diff(0)
	, m_ask(0), m_askSize(0)
	, m_bid(0), m_bidSize(0)
{
	m_priceTick = strategyItem.sc_pricetick();
	m_caseLE2Tick = strategyItem.sc_casele2tick();
	m_caseLE3Tick = strategyItem.sc_casele3tick();
	m_caseGE4Tick = strategyItem.sc_casege4tick();
	m_caseNoChange = strategyItem.sc_casenochange();
	m_stopLossStrategy = strategyItem.sc_stoplossstrategy();

	CreateTriggers(strategyItem);
}


CScalperStrategy::~CScalperStrategy(void)
{
}

void CScalperStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	if(strategyItem.triggers_size() > 0)
	{
		m_triggers.push_back(TriggerPtr(new CScalperTrigger(strategyItem.triggers(0))));
	}
}

void CScalperStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	m_diff = pQuote->ask() - pQuote->bid();
	m_ask = pQuote->ask();
	m_askSize = pQuote->ask_size();
	m_bid = pQuote->bid();
	m_bidSize = pQuote->bid_size();

	if(m_askSize > 0 && m_bidSize > 0)
	{
		if(m_triggers[0]->Test(m_diff))
		{
			// open position
			// pPortfolio->OrderPlacer()->Run();
		}
	}

	// there is only ONE leg for scalper strategy 
	const LegPtr& leg = pPortfolio->Legs().at(0);
	leg->UpdateLast(pQuote->last());
	leg->UpdateAsk(pQuote->ask());
	leg->UpdateAskSize(pQuote->ask_size());
	leg->UpdateBid(pQuote->bid());
	leg->UpdateBidSize(pQuote->bid_size());
	leg->UpdateTimestamp();
}

void CScalperStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	pPortfUpdateItem->set_sc_diff(m_diff);
}
