#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioArbitrageOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioArbitrageOrderPlacer(void);
	~CPortfolioArbitrageOrderPlacer(void);

	virtual void BuildTemplateOrder();

	void OpenPosition();
	void ClosePosition();
	bool IsOpened();

private:

};

