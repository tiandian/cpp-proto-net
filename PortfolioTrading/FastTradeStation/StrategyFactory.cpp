#include "StrategyFactory.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"
#include "ScalperStrategy.h"
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
	else if(name == "Scalper")
	{
		pStrategy = new CScalperStrategy();
		pPortf->SelfClose(true);
	}
	
	_ASSERT(pStrategy != NULL);
	pStrategy->Portfolio(pPortf);
	pStrategy->ApplySettings(data);

	return pStrategy;
}
