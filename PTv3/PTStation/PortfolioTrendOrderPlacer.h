#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioTrendOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioTrendOrderPlacer(void);
	~CPortfolioTrendOrderPlacer(void);

	void CloseOrder(double limitprice);

	bool IsOpened();
	bool IsClosing();

protected:
	virtual void BuildTemplateOrder();
	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer);
	virtual void OnLegOrderSent(int orderPlacerIdx);
	virtual void OnPortfolioDone();

private:
	int m_sentOrderIdx;

};

