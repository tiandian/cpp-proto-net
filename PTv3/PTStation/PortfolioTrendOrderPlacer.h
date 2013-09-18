#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioTrendOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioTrendOrderPlacer(void);
	~CPortfolioTrendOrderPlacer(void);

	virtual void BuildTemplateOrder();

	virtual void SetDirection(trade::PosiDirectionType posiDirection){}
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize){}

	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer){}
};

