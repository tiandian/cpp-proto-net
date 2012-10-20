#include "StdAfx.h"
#include "ArbitrageStrategy.h"
#include "multilegorderptr.h"

#include <string>
#include <vector>
#include <boost/foreach.hpp>

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
}

void CArbitrageStrategy::DoOpenPostion()
{
	CPortfolio *portf = Portfolio();
	if(portf != NULL)
		Client()->OpenPosition(portf, portf->Quantity());
}

void CArbitrageStrategy::CloseAllPosition()
{
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
