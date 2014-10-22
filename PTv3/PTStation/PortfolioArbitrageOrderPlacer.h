#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioArbitrageOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioArbitrageOrderPlacer(void);
	~CPortfolioArbitrageOrderPlacer(void);

	void OpenPosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, trade::SubmitReason reason);
	void ClosePosition(int volumeToClose, entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, trade::SubmitReason reason);
	bool IsOpened();

	void Run(entity::PosiDirectionType posiDirection, int quantity, trade::OffsetFlagType offset, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, const char* openOrderId = NULL);
	
protected:
	virtual void BuildTemplateOrder();
	virtual CLegOrderPlacer* CreateLegOrderPlacer(int openTimeout, int maxRetryTimes);
	virtual void OnPortfolioDone(PortfolioFinishState portfState);

private:
	CLegOrderPlacer* GetLegOrderPlacer(const string& symbol);
	
	bool m_openingPosition;
	bool m_openedPosition;
	string m_lastOpenOrderId;
};

