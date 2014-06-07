#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioArbitrageOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioArbitrageOrderPlacer(void);
	~CPortfolioArbitrageOrderPlacer(void);

	virtual void BuildTemplateOrder();

	void OpenPosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp);
	void ClosePosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp);
	bool IsOpened();

	void Run(entity::PosiDirectionType posiDirection, trade::OffsetFlagType offset, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp);
private:

	CLegOrderPlacer* GetLegOrderPlacer(const string& symbol);
};

