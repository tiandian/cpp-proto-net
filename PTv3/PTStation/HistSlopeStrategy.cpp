#include "StdAfx.h"
#include "HistSlopeStrategy.h"
#include "HistSlopeTrigger.h"
#include "AvatarClient.h"

CHistSlopeStrategy::CHistSlopeStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
{
	Apply(strategyItem, false);

	CreateTriggers(strategyItem);
}


CHistSlopeStrategy::~CHistSlopeStrategy(void)
{
}

void CHistSlopeStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	CTechAnalyStrategy::Apply(strategyItem, withTriggers);
	m_stdHist = strategyItem.hs_stdhist();
}

void CHistSlopeStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		TriggerPtr trigger(new CHistSlopeTrigger(strategyItem.triggers(i)));
		m_triggers.push_back(trigger);
	}
}

void CHistSlopeStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);
   
   // 1. Calculate MACD hist value of 1 min and 5 min
   
   // 2. Test 5 min angle, see if Up or Down.
   
   // 3. Calculate value of 1 min angle
   // 3.1 if sign of 1 min is same as 5 min, Goes to Trigger test
   
   // 3.2 In scope of Trigger test
   // 3.2.1 if not opened, test open trigger
   // 3.2.2 if open tigger is fired, be sure to enable trailing stop trigger with Enable(cost, direction)
   
   // 3.2.2 else, test close trigger
   
   // 3.3 else sign of 1 min is different than 5 min, Goes to close existing position if any
   
}

void CHistSlopeStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	
	pPortfUpdateItem->set_hs_fastangel(45);
	pPortfUpdateItem->set_hs_slowangel(30);
}

int CHistSlopeStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	return 0;
}

int CHistSlopeStrategy::OnPortfolioRemovePosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder )
{
	return 0;
}

void CHistSlopeStrategy::CalculateAngel()
{

}

