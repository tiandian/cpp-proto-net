#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioTrendOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioTrendOrderPlacer(void);
	~CPortfolioTrendOrderPlacer(void);

	void CloseOrder(double limitprice);

	virtual void BuildTemplateOrder();
	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer){}

};

