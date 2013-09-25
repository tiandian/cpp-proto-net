#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioScalperOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioScalperOrderPlacer(void);
	~CPortfolioScalperOrderPlacer(void);

	virtual void BuildTemplateOrder();

	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer);
};

