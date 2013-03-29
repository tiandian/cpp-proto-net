#pragma once

#include "MLOrderStateMachine.h"

#include <boost/shared_ptr.hpp>

class CAsyncOpenOrderTimer;
class CAsyncNextQuoteWaiter;
class op2::COrderEvent;

class CScalperPortfolioOrderPlacer : public CMLOrderPlacer
{
public:
	CScalperPortfolioOrderPlacer(
		CMLOrderStateMachine* pStateMachine,
		CPortfolio* pPortf,
		const MultiLegOrderPtr& mlOrder,
		COrderProcessor2* pOrdProcessor):
	CMLOrderPlacer(pStateMachine, pPortf, mlOrder, pOrdProcessor)
	{}
	~CScalperPortfolioOrderPlacer(void){}

protected:
	virtual void Send();
	virtual bool SendNext(op2::COrderEvent* transEvent);

private:
	boost::shared_ptr<CAsyncOpenOrderTimer> m_openOrderTimer;
	boost::shared_ptr<CAsyncNextQuoteWaiter> m_nextQuoteWaiter;
};

