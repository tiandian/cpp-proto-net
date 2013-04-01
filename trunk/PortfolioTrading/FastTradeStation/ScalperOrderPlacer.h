#pragma once

#include "AsyncScalperEventFirer.h"
#include "SgOrderStateMachine.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

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
	virtual void OnCanceling(COrderEvent* transEvent);

private:
	bool IsOpenOrder();
	void WaitForPendingOrderProc();
	bool IsPendingOrderReady(){ return m_pendingOrder.get() != NULL; }

	double m_precedence;
	entity::StopLossCloseMethods m_closeMethod;
	boost::shared_ptr<trade::Order> m_pendingOrder;

	boost::shared_ptr<CAsyncScalperEventFirer> m_openOrderTimer;
	boost::shared_ptr<CAsyncScalperEventFirer> m_nextQuoteWaiter;

	double m_nextLast;
	double m_nextAsk;
	double m_nextBid;

	boost::condition_variable m_condCancelingWaitPendingOrder;
	boost::mutex m_mut;
};

