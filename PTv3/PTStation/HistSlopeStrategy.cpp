#include "StdAfx.h"
#include "HistSlopeStrategy.h"
#include "HistSlopeTrigger.h"
#include "AvatarClient.h"
#include "TaIndicatorSet.h"

#define PI 3.1415926

TaIndicatorSetPtr CalculateMACD(COHLCRecordSet* ohlcRecordSet)
{
	return TaIndicatorSetPtr();
}

MACDSlopeDirection CheckDirection(double point1, double point2)
{
	if(point2 - point1 > 0.01)
		return GOING_UP;
	else if(point1 - point2 > 0.01)
		return GOING_DOWN;
	else
		return NO_DIRECTION;
}

CHistSlopeStrategy::CHistSlopeStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_stdHistDiff(0)
	, m_angel(0)
	, m_positionOpened(false)
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
	m_stdHistDiff = strategyItem.hs_stdhist();
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
	string symbol = pQuote->symbol();

	COHLCRecordSet* slowOHLC = GetRecordSet(symbol, 300, timestamp);
	TaIndicatorSetPtr slowIndicatorSet = CalculateMACD(slowOHLC);

	double last0 = slowIndicatorSet->GetRef("MACD", 0);
	double last1 = slowIndicatorSet->GetRef("MACD", 1);
	// 2. Test 5 min angle, see if Up or Down.
	MACDSlopeDirection slowPeriodDirection = CheckDirection(last0, last1);

	// 3. Calculate value of 1 min angle
	COHLCRecordSet* fastOHLC = GetRecordSet(symbol, 60, timestamp);
	TaIndicatorSetPtr fastIndicatorSet = CalculateMACD(fastOHLC);
	// 3.1 if sign of 1 min is same as 5 min, Goes to Trigger test
	double fastLast0 = fastIndicatorSet->GetRef("MACD", 0);
	double fastLast1 = fastIndicatorSet->GetRef("MACD", 1);

	MACDSlopeDirection fastPeriodDirection = CheckDirection(fastLast0 , fastLast1);
	if(fastPeriodDirection == slowPeriodDirection )
	{
		double fastAngle = CalculateAngle(fastLast1, fastLast0);
		// 3.2 In scope of Trigger test
		for(TriggerIter iter = m_triggers.begin(); iter != m_triggers.end(); ++iter)
		{
			CHistSlopeTrigger* slopeTrigger = dynamic_cast<CHistSlopeTrigger*>(iter->get());
			if(m_positionOpened)
			{
				// 3.2.1 if not opened, test open trigger
				if(slopeTrigger->Offset() == entity::OPEN)
				{
					bool meetOpenCondition = slopeTrigger->Test(fastAngle);
					if(meetOpenCondition)
					{
						// 3.2.1.1 Do OPEN position

						// 3.2.1.2 if open trigger is fired, be sure to enable trailing stop trigger with Enable(cost, direction)
						break;
					}
				}
			}
			else
			{
				// 3.2.2 else, test close trigger
				if(slopeTrigger->Offset() == entity::CLOSE)
				{
					bool meetCloseCondition = slopeTrigger->Test(fastAngle);
					if(meetCloseCondition)
					{
						// TODO Close position

						break;
					}
				}
			}
		}
	}
	else
	{
		// 3.3 else sign of 1 min is different than 5 min, Goes to close existing position if any
		// TODO Close position

	}
}

void CHistSlopeStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	
	pPortfUpdateItem->set_hs_fastangel(45);
	pPortfUpdateItem->set_hs_slowangel(30);
}

int CHistSlopeStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	m_positionOpened = true;
	return 0;
}

int CHistSlopeStrategy::OnPortfolioRemovePosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder )
{
	m_positionOpened = false;
	return 0;
}

double CHistSlopeStrategy::CalculateAngle(double val1, double val2)
{
    double diff = val2 - val1;
    double arcTan = atan(diff);
    double angle = arcTan * 180 / PI;
    return angle;
}

