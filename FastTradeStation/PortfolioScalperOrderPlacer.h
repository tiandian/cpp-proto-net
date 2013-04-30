#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioScalperOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioScalperOrderPlacer(void);
	~CPortfolioScalperOrderPlacer(void);

	virtual void BuildTemplateOrder();

	virtual void SetDirection(trade::PosiDirectionType posiDirection);
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize);

	virtual void OnLegCanceled(trade::Order* pRtnOrder);
};

