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
	virtual void OnLegOrderFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume);
	virtual void OnPortfolioDone();

	int m_sentOrderIdx;

};

