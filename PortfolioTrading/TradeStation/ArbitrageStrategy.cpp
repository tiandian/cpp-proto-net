#include "StdAfx.h"
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
		Client()->OpenPosition(portf, portf->Quantity());
}

void CArbitrageStrategy::CloseAllPosition()
{
	logger.Info(boost::str(boost::format("Portf (%s) CLOSE position by Arbitrage strategy") % Portfolio()->ID().c_str()));

	CPortfolio *portf = Portfolio();
	if(portf != NULL)
	{
		std::vector<MultiLegOrderPtr> openedOrders;
		int orderCount = portf->GetPosition(openedOrders);
		if(orderCount > 0)
		{
			BOOST_FOREACH(MultiLegOrderPtr mlOrd, openedOrders)
			{
				const trade::MultiLegOrder& openMlOrd = *mlOrd;
				std::string msg;
				Client()->ClosePosition(openMlOrd, std::string(), msg);
			}
		}
	}
}
