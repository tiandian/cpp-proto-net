#include "ArbitrageStrategy.h"
#include "multilegorderptr.h"
#include "globalmembers.h"

#include <string>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

CArbitrageStrategy::CArbitrageStrategy(void)
{
}


CArbitrageStrategy::~CArbitrageStrategy(void)
{
}

void CArbitrageStrategy::ApplySettings( const std::string& settingData )
{
	entity::ArbitrageStrategySettings arbitrageSettings;
	arbitrageSettings.ParseFromString(settingData);

	Direction(arbitrageSettings.side());

	SetOpenPosiCond(ConvertCompareCondition(arbitrageSettings.opencondition()), arbitrageSettings.openposithreshold());
	SetStopGainCond(ConvertCompareCondition(arbitrageSettings.stopgaincondition()), arbitrageSettings.stopgainthreshold());
	SetStopLossCond(ConvertCompareCondition(arbitrageSettings.stoplosscondition()), arbitrageSettings.stoplossthreshold());

	logger.Info(boost::str(boost::format("Threshold: Open %f, StopGain: %f, StopLoss: %f")
		% arbitrageSettings.openposithreshold() % arbitrageSettings.stopgainthreshold() % arbitrageSettings.stoplossthreshold()));
}

void CArbitrageStrategy::DoOpenPostion()
{
	logger.Info(boost::str(boost::format("Portf (%s) OPEN position by Arbitrage strategy") % Portfolio()->ID().c_str()));

	CPortfolio *portf = Portfolio();
	if(portf != NULL)
		Client()->OpenPosition(portf, portf->Quantity(), trade::SR_AutoOpen);
}

void CArbitrageStrategy::ClosePosition(trade::SubmitReason submitReason)
{
	logger.Info(boost::str(boost::format("Portf (%s) CLOSE position by Arbitrage strategy") % Portfolio()->ID().c_str()));

	CPortfolio *portf = Portfolio();
	if(portf != NULL)
	{
		int qty = portf->Quantity();
		int positionQty = portf->PositionQuantity();
		_ASSERT(positionQty > 0);
		
		const string& portfId = portf->ID();
		Client()->ClosePosition(portfId, qty, submitReason);
	}
}

POSI_OPER CArbitrageStrategy::NextOperation( POSI_OPER oper )
{
	switch (oper)
	{
	case OPEN_POSI:
		return CLOSE_POSI;
	case CLOSE_POSI:
		return DO_NOTHING;
	default:
		return DO_NOTHING;
	}
}

void CArbitrageStrategy::Test()
{
	if(!m_isRunning) return;

	entity::PosiDirectionType direction = Direction();
	if(direction == entity::LONG)
	{
		double longDiff = Portfolio()->LongDiff();
		CStrategy<double>::Test(longDiff, OPEN_POSI);
		
		double shortDiff = Portfolio()->ShortDiff();
		CStrategy<double>::Test(shortDiff, CLOSE_POSI);
	}
	else if(direction == entity::SHORT)
	{
		double shortDiff = Portfolio()->ShortDiff();
		CStrategy<double>::Test(shortDiff, OPEN_POSI);

		double longDiff = Portfolio()->LongDiff();
		CStrategy<double>::Test(longDiff, CLOSE_POSI);
	}
	else
		CStrategy<double>::Test(Portfolio()->Difference());
}
