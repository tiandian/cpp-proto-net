#pragma once
#include "PortfolioTrendOrderPlacer.h"


class CManualOrderPlacer :
	public CPortfolioTrendOrderPlacer
{
public:
	CManualOrderPlacer();
	virtual ~CManualOrderPlacer();

	virtual void Prepare();

	virtual void Cleanup(){}

	virtual bool IfPortfolioCanceled(){ return false; }

protected:

	virtual CLegOrderPlacer* CreateLegOrderPlacer(int openTimeout, int maxRetryTimes);
};

