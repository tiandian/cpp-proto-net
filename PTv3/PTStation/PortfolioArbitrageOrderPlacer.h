#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioArbitrageOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioArbitrageOrderPlacer(void);
	~CPortfolioArbitrageOrderPlacer(void);

	void OpenPosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, trade::SubmitReason reason);
	void ClosePosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, trade::SubmitReason reason);
	bool IsOpened();

	void Run(entity::PosiDirectionType posiDirection, trade::OffsetFlagType offset, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, const char* openOrderId = NULL);

protected:
	virtual void BuildTemplateOrder();

private:
	CLegOrderPlacer* GetLegOrderPlacer(const string& symbol);
	
	bool m_openedPosition;
	string m_lastOpenOrderId;
};

