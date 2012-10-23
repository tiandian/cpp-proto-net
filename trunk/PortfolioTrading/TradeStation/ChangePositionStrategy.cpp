#include "StdAfx.h"
#include "ChangePositionStrategy.h"
#include "globalmembers.h"

#include <boost/format.hpp>


CChangePositionStrategy::CChangePositionStrategy(void):
m_closingLegPosition(entity::LONG)
{
}


CChangePositionStrategy::~CChangePositionStrategy(void)
{
}

void CChangePositionStrategy::ApplySettings( const std::string& settingData )
{
	entity::ChangePosiStrategySettings settings;
	settings.ParseFromString(settingData);

	CloseLeg(settings.closeleg());
	m_closingLegPosition = settings.closelegside();
	SetOpenPosiCond(ConvertCompareCondition(settings.triggercondition()), 
											settings.threshold());

	logger.Info(boost::str(boost::format("Symbol to close: %s, Side: %d, Threshold: %f") 
		% CloseLeg().c_str() % m_closingLegPosition % settings.threshold()));
}

void CChangePositionStrategy::DoOpenPostion()
{
	logger.Info(boost::str(boost::format("Portf (%s) Changing position by strategy") % Portfolio()->ID().c_str()));

	CPortfolio *portf = Portfolio();
	if(portf != NULL)
		Client()->ChangePosition(portf, CloseLeg(), portf->Quantity());
}

POSI_OPER CChangePositionStrategy::NextOperation( POSI_OPER oper )
{
	switch (oper)
	{
	case OPEN_POSI:
		return DO_NOTHING;
	default:
		return DO_NOTHING;
	}
}

void CChangePositionStrategy::Test()
{
	if(!m_isRunning) return;

	if(m_testingFor == OPEN_POSI)
	{
		CLeg* pLeg = Portfolio()->GetLeg(CloseLeg());
		if(pLeg != NULL)
		{
			entity::PosiDirectionType side = pLeg->Side();
			if(side == entity::LONG)
			{
				if(m_closingLegPosition == entity::LONG)
				{
					double shortDiff = Portfolio()->ShortDiff();
					CStrategy<double>::Test(shortDiff);
				}
				else if(m_closingLegPosition == entity::SHORT)
				{
					double longDiff = Portfolio()->LongDiff();
					CStrategy<double>::Test(longDiff);
				}
			}
			else if(side == entity::SHORT)
			{
				if(m_closingLegPosition == entity::LONG)
				{
					double longDiff = Portfolio()->LongDiff();
					CStrategy<double>::Test(longDiff);
				}
				else if(m_closingLegPosition == entity::SHORT)
				{
					double shortDiff = Portfolio()->ShortDiff();
					CStrategy<double>::Test(shortDiff);
				}
			}
		}
	}
}
