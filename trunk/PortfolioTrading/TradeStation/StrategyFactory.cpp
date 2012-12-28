#include "StdAfx.h"
#include "StrategyFactory.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"
#include "Portfolio.h"

CDiffStrategy* CreateStrategy( CPortfolio* pPortf, const string& name, const string& data )
{
	CDiffStrategy* pStrategy = NULL;
	if(name == "ArbitrageStrategy")
	{
		pStrategy = new CArbitrageStrategy();
	}
	else if(name == "ChangePosition")
	{
		pStrategy = new CChangePositionStrategy();
	}
	
	pStrategy->Portfolio(pPortf);
	pStrategy->ApplySettings(data);

	return pStrategy;
}
