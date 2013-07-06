#include "StdAfx.h"
#include "ArbitrageStrategy.h"
#include "ArbitrageTrigger.h"
#include "Portfolio.h"

enum DIFF_TYPE 
{
	LAST_DIFF, LONG_DIFF, SHORT_DIFF
};

double CalcDiff(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	boost::chrono::steady_clock::time_point tp = boost::chrono::steady_clock::now();

	// calculate the diff
	double diff = 0;
	for(vector<LegPtr>::iterator iter = legs.begin(); iter != legs.end(); ++iter)
	{
		entity::PosiDirectionType legSide = (*iter)->Side();
		double legPrice = 0;
		if(legSide == entity::LONG)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legPrice = (*iter)->Ask();
				break;
			case SHORT_DIFF:
				legPrice = (*iter)->Bid();
				break;
			case LAST_DIFF:
			default:
				legPrice = (*iter)->Last();
			}
		}
		else if(legSide == entity::SHORT)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legPrice = (*iter)->Bid();
				break;
			case SHORT_DIFF:
				legPrice = (*iter)->Ask();
				break;
			case LAST_DIFF:
			default:
				legPrice = (*iter)->Last();
			}
		}

		if(legPrice > 0)
		{
			if((*iter)->Side() == entity::LONG)
			{
				diff +=	legPrice;
			}
			else
				diff -= legPrice;
		}
		else	// if one of legs has no price, set diff 0 anyway
			diff = 0;
	}
	return diff;
}

int CalcSize(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	// calculate the diff
	int diffSize = 0;
	for(vector<LegPtr>::iterator iter = legs.begin(); iter != legs.end(); ++iter)
	{
		int legSize = 0;
		entity::PosiDirectionType legSide = (*iter)->Side();
		if(legSide == entity::LONG)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legSize = (*iter)->AskSize();
				break;
			case SHORT_DIFF:
				legSize = (*iter)->BidSize();
				break;
			}
		}
		else if(legSide == entity::SHORT)
		{
			switch(diffType)
			{
			case LONG_DIFF:
				legSize = (*iter)->BidSize();
				break;
			case SHORT_DIFF:
				legSize = (*iter)->AskSize();
				break;
			}
		}

		if(legSize > 0)
		{
			if(diffSize == 0) diffSize = legSize;
			else
				diffSize = legSize < diffSize ? legSize : diffSize;
		}
	}
	return diffSize;
}

CArbitrageStrategy::CArbitrageStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
	, m_lastDiff(0)
	, m_longDiff(0)
	, m_longDiffSize(0)
	, m_shortDiff(0)
	, m_shortDiffSize(0)
{
	m_side = strategyItem.ar_side();

	CreateTriggers(strategyItem);
}


CArbitrageStrategy::~CArbitrageStrategy(void)
{
}

void CArbitrageStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		TriggerPtr trigger(new CArbitrageTrigger(strategyItem.triggers(i)));
		m_triggers.push_back(trigger);
	}
}

void CArbitrageStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	CStrategy::Test(pQuote, pPortfolio, timestamp);
	vector<LegPtr>& vecLegs = pPortfolio->Legs();

	m_lastDiff = CalcDiff(vecLegs, LAST_DIFF);
	m_longDiff = CalcDiff(vecLegs, LONG_DIFF);
	m_longDiffSize = CalcSize(vecLegs, LONG_DIFF);
	m_shortDiff = CalcDiff(vecLegs, SHORT_DIFF);
	m_shortDiffSize = CalcSize(vecLegs, SHORT_DIFF);
}

void CArbitrageStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	pPortfUpdateItem->set_diff(m_lastDiff);
	pPortfUpdateItem->set_longdiff(m_longDiff);
	pPortfUpdateItem->set_longsize(m_longDiffSize);
	pPortfUpdateItem->set_shortdiff(m_shortDiff);
	pPortfUpdateItem->set_shortsize(m_shortDiffSize);
}


