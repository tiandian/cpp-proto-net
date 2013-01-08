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
		:m_pStateMachine(pStateMachine), m_pPortf(pPortf),
		m_mlOrder(mlOrder), m_pOrderProcessor(pOrdProcessor)
	{}
	~CMLOrderPlacer(){}

	const string& Id(){ return m_mlOrder->orderid(); }

	void Do();

private:

	void Send();
	void CreateSgOrderPlacer(const boost::shared_ptr<trade::InputOrder>& inputOrder);

	CMLOrderStateMachine* m_pStateMachine;
	CPortfolio* m_pPortf;
	MultiLegOrderPtr m_mlOrder;
	COrderProcessor2* m_pOrderProcessor;

	std::vector<OrderPlacerPtr> m_sgOrderPlacers;
};

class CMLOrderStateMachine : public COrderStateMachine
{
public:
	CMLOrderStateMachine(void);
	~CMLOrderStateMachine(void);

	OrderPlacerPtr CreatePlacer(CPortfolio* pPortf, const MultiLegOrderPtr& mlOrder, COrderProcessor2* pOrdProc)
	{
		return OrderPlacerPtr(new CMLOrderPlacer(this, pPortf, mlOrder, pOrdProc));
	}

	void Init();
	//void Transition(const string& orderId, COrderEvent& event);
};

