#pragma once

#include "multilegorderptr.h"
#include "OrderStateMachine.h"

#include <vector>

class CMLOrderStateMachine;
class CPortfolio;
class COrderProcessor2;
class CSgOrderPlacer;

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
	~CMLOrderPlacer()
	{

	}

	bool OnEnter(ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState);

	const string& Id(){ return m_mlOrder->orderid(); }
	void Do();

private:

	void Send();
	bool SendNext(COrderEvent* transEvent);
	CSgOrderPlacer* CreateSgOrderPlacer(const boost::shared_ptr<trade::InputOrder>& inputOrder, int retryTimes);
	void OutputStatus(const string& statusMsg);

	CMLOrderStateMachine* m_pStateMachine;
	CPortfolio* m_pPortf;
	MultiLegOrderPtr m_mlOrder;
	COrderProcessor2* m_pOrderProcessor;

	std::vector<CSgOrderPlacer*> m_sgOrderPlacers;
	bool m_isSequential;
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

	virtual void Initialize();
	void Transition(const string& orderId, COrderEvent& event);
};

namespace op2
{
	class CMLegOrderEvent : public COrderEvent
	{
	public:
		CMLegOrderEvent(const std::string& symbol, ORDER_EVENT ordEvt):
		  COrderEvent(ordEvt), m_legSymbol(symbol){}
		virtual ~CMLegOrderEvent(){}

		const string& LegSymbol(){ return m_legSymbol; }

	protected:
		std::string m_legSymbol;
	};

	class LegCompletedEvent : public CMLegOrderEvent
	{
	public:
		LegCompletedEvent(const std::string& symbol, int remained, int finished):
		  CMLegOrderEvent(symbol, ORDER_EVENT_COMPLETE),
			  m_remained(remained), m_finished(finished)
		  {}

		int Remained(){ return m_remained; }
		int Finished(){ return m_finished; }

	private:
		int m_remained;
		int m_finished;
	};

	class LegCanceledEvent : public CMLegOrderEvent
	{
	public:
		LegCanceledEvent(const std::string& symbol):CMLegOrderEvent(symbol, ORDER_EVENT_CANCEL_SUCCESS){}
	};

	class LegRejectedEvent : public CMLegOrderEvent
	{
	public:
		LegRejectedEvent(const std::string& symbol):CMLegOrderEvent(symbol, ORDER_EVENT_SUBMIT_FAILED){}
	};
};

