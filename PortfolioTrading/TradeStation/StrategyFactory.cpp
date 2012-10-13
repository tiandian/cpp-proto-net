#include "StdAfx.h"
#include "StrategyFactory.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"

CDiffStrategy* CreateStrategy( const string& name, const string& data )
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
	
	pStrategy->ApplySettings(data);

	return pStrategy;
}
