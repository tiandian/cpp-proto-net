#pragma once

#include "SgOrderStateMachine.h"

#include <boost/shared_ptr.hpp>

class CAsyncOpenOrderTimer;
class CAsyncNextQuoteWaiter;

class CScalperOrderPlacer : public CSgOrderPlacer
{
public:
	CScalperOrderPlacer(CSgOrderStateMachine* pStateMachine,
		CPortfolio* pPortfolio,
		trade::MultiLegOrder* pMultiLegOrder,
		const InputOrderPtr& inputOrder,
		int maxRetryTimes,
		COrderProcessor2* pOrderProc);

	~CScalperOrderPlacer(){}

	double Precedence() const { return m_precedence; }
	void SetPrecedence(double val) { m_precedence = val; }

protected:
	virtual void ModifyOrderPrice();

	virtual void OnSubmittingOrder();
	virtual void OnPending(trade::Order* pOrd);
	virtual void OnCanceling();

private:
	bool IsOpenOrder();

	double m_precedence;
	entity::StopLossCloseMethods m_closeMethod;
	trade::Order* m_pendingOrder;

	boost::shared_ptr<CAsyncOpenOrderTimer> m_openOrderTimer;
	boost::shared_ptr<CAsyncNextQuoteWaiter> m_nextQuoteWaiter;
};

