#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioScalperOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioScalperOrderPlacer(CPortfolio* pPortf, COrderProcessor2* pOrderProc);
	~CPortfolioScalperOrderPlacer(void);

	virtual void BuildTemplateOrder();

	virtual void SetDirection(trade::PosiDirectionType posiDirection);
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize);
};

