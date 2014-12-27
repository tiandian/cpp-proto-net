#include "StdAfx.h"
#include "ManualOrderPlacer.h"


CManualOrderPlacer::CManualOrderPlacer()
{
}


CManualOrderPlacer::~CManualOrderPlacer()
{
}

void CManualOrderPlacer::Prepare()
{
	if (IsReadyForPrepare())
		CPortfolioOrderPlacer::Prepare();
}

CLegOrderPlacer* CManualOrderPlacer::CreateLegOrderPlacer(int openTimeout, int maxRetryTimes)
{
	return new CManualLegOrderPlacer(this, openTimeout, maxRetryTimes);
}
