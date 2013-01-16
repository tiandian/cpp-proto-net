#pragma once

#include "multilegorderptr.h"
#include "OrderStateMachine.h"

#include <vector>

class CMLOrderStateMachine;
class CPortfolio;
class COrderProcessor2;

class CMLOrderPlacer : public COrderPlacer
{
public:
	CMLOrderPlacer(CMLOrderStateMachine* pStateMachine,
				   CPortfolio* pPortf,
				   const MultiLegOrderPtr& mlOrder,
				   COrderProcessor2* pOrdProcessor)
		:m_pStateMachine(pStateMachine), m_pPortf(pPortf), m_sendingIdx(-1),
		m_isSequential(false), m_mlOrder(mlOrder), m_pOrderProcessor(pOrdProcessor)
	{}
	~CMLOrderPlacer(){}

	bool OnEnter(ORDER_STATE state, COrderEvent* transEvent);

	const string& Id(){ return m_mlOrder->orderid(); }
	void Do();

private:

	void Send();
	void SendNext();
	COrderPlacer* CreateSgOrderPlacer(const boost::shared_ptr<trade::InputOrder>& inputOrder, int retryTimes);

	CMLOrderStateMachine* m_pStateMachine;
	CPortfolio* m_pPortf;
	MultiLegOrderPtr m_mlOrder;
	COrderProcessor2* m_pOrderProcessor;

	std::vector<COrderPlacer*> m_sgOrderPlacers;
	int m_isSequential;
	int m_sendingIdx;
};

class CMLOrderStateMachine : public COrderStateMachine
{
public:
	CMLOrderStateMachine(void);
	~CMLOrderStateMachine(void);

	COrderPlacer* CreatePlacer(CPortfolio* pPortf, const MultiLegOrderPtr& mlOrder, COrderProcessor2* pOrdProc)
	{
		return new CMLOrderPlacer(this, pPortf, mlOrder, pOrdProc);
	}

	void Init();
	void Transition(const string& orderId, COrderEvent& event);
};

namespace op2
{
	class LegCompletedEvent : public COrderEvent
	{
	public:
		LegCompletedEvent():COrderEvent(ORDER_EVENT_COMPLETE){}
	};

	class LegCanceledEvent : public COrderEvent
	{
	public:
		LegCanceledEvent():COrderEvent(ORDER_EVENT_CANCEL_SUCCESS){}
	};

	class LegRejectedEvent : public COrderEvent
	{
	public:
		LegRejectedEvent():COrderEvent(ORDER_EVENT_REJECTED){}
	};
};

