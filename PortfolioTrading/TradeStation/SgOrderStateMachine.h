#pragma once

#include "OrderState.h"
#include "OrderStateMachine.h"
#include "orderhelper.h"
#include "../Entity/gen/cpp/trade.pb.h"

class CSgOrderStateMachine;
class COrderProcessor2;
class CPortfolio;

typedef boost::shared_ptr<trade::InputOrder> InputOrderPtr;

class CSgOrderPlacer : public COrderPlacer
{
public:
	CSgOrderPlacer(CSgOrderStateMachine* pStateMachine,
					CPortfolio* pPortfolio,
					trade::MultiLegOrder* pMultiLegOrder,
					const InputOrderPtr& inputOrder,
					int maxRetryTimes,
					COrderProcessor2* pOrderProc)
					:m_pStateMachine(pStateMachine),
					m_pPortf(pPortfolio),
					m_pMultiLegOrder(pMultiLegOrder),
					m_pInputOrder(inputOrder),
					m_maxRetryTimes(maxRetryTimes),
					m_submitTimes(0), m_succ(false), m_allowPending(false),
					m_pOrderProcessor(pOrderProc)
	{
		GetOrderUid(inputOrder.get(), m_sgOrderUid);
	}
	~CSgOrderPlacer()
	{

	}

	const string& ParentOrderId(){ return m_pMultiLegOrder != NULL ? m_pMultiLegOrder->orderid() : EmptyParentOrderId; }
	const string& Symbol() { return m_pInputOrder->instrumentid(); }
	const string& Id(){ return m_currentOrdRef; }
	const string& CompositeId() { return m_sgOrderUid; }

	bool OnEnter(ORDER_STATE state, COrderEvent* transEvent);

	void Do();

protected:
	virtual void RaiseMultiLegOrderEvent(COrderEvent& orderEvent);
	virtual void OnOrderUpdate(trade::Order* pOrd);
	virtual void ModifyOrderPrice();
	virtual void OnOrderPlaceFailed(COrderEvent* pOrdEvent);

	static string EmptyParentOrderId;

	string m_sgOrderUid;
	string m_currentOrdRef;
	CSgOrderStateMachine* m_pStateMachine;
	trade::MultiLegOrder* m_pMultiLegOrder;
	CPortfolio* m_pPortf;
	InputOrderPtr m_pInputOrder;
	COrderProcessor2* m_pOrderProcessor;
	
	int m_maxRetryTimes;
	int m_submitTimes;
	bool m_succ;
	string m_errorMsg;
	bool m_allowPending;
};

class CManualSgOrderPlacer : public CSgOrderPlacer
{
public:
	CManualSgOrderPlacer(CSgOrderStateMachine* pStateMachine,
		const InputOrderPtr& pInputOrder,
		int retryTimes,
		COrderProcessor2* pOrderProc):
	CSgOrderPlacer(pStateMachine, NULL, NULL, 
		pInputOrder, retryTimes, pOrderProc){}

	~CManualSgOrderPlacer(){}

	bool DoAndWait();
	const string& GetError(){ return m_errorMsg; }
	bool AutoDispose() { return false; }

protected:
	virtual void RaiseMultiLegOrderEvent(COrderEvent& orderEvent);
	virtual void OnOrderUpdate(trade::Order* pOrd){}
	virtual void ModifyOrderPrice();
	virtual void OnOrderPlaceFailed(COrderEvent* pOrdEvent){}

private:
	boost::condition_variable m_cond;
	boost::mutex m_mut;
};

class CSgOrderStateMachine : public COrderStateMachine
{
public:
	CSgOrderStateMachine(void);
	~CSgOrderStateMachine(void);

	COrderPlacer* CreatePlacer( CPortfolio* pPortfolio,
								trade::MultiLegOrder* pMultiLegOrder,
								const InputOrderPtr& pInputOrder,
								int retryTimes,
								COrderProcessor2* pOrderProc)
	{
		return new CSgOrderPlacer(this, pPortfolio, pMultiLegOrder, pInputOrder, retryTimes, pOrderProc);
	}

	CManualSgOrderPlacer* CreateManualPlacer(const InputOrderPtr& pInputOrder,
											int retryTimes,
											COrderProcessor2* pOrderProc)
	{
		return new CManualSgOrderPlacer(this, pInputOrder, retryTimes, pOrderProc);
	}

	virtual void Initialize();
	void Transition(const string& orderId, COrderEvent& event);
};

namespace op2
{
	class CSgOrderEvent : public COrderEvent
	{
	public:
		CSgOrderEvent(ORDER_EVENT evt, trade::Order* rtnOrder):
		  COrderEvent(evt), m_rtnOrder(rtnOrder)
		  {}

		  trade::Order* RtnOrder(){ return m_rtnOrder; }

	private:
		trade::Order* m_rtnOrder;
	};

	class CompleteEvent : public CSgOrderEvent
	{
	public:
		CompleteEvent(trade::Order* rtnOrder):CSgOrderEvent(ORDER_EVENT_COMPLETE, rtnOrder){}
	};

	class RejectEvent: public CSgOrderEvent
	{
	public:
		RejectEvent(trade::Order* rtnOrder) : CSgOrderEvent(ORDER_EVENT_REJECTED, rtnOrder){}
	};

	class PendingEvent : public CSgOrderEvent
	{
	public:
		PendingEvent(trade::Order* rtnOrder):CSgOrderEvent(ORDER_EVENT_PENDING, rtnOrder){}
	};

	class CancelSuccessEvent : public CSgOrderEvent
	{
	public:
		CancelSuccessEvent(trade::Order* rtnOrder):CSgOrderEvent(ORDER_EVENT_CANCEL_SUCCESS, rtnOrder){}
	};

	class CancelFailedEvent : public CSgOrderEvent
	{
	public:
		CancelFailedEvent(trade::Order* rtnOrder):CSgOrderEvent(ORDER_EVENT_CANCEL_FAILED, rtnOrder){}
	};

	class PartiallyFilledEvent : public CSgOrderEvent
	{
	public:
		PartiallyFilledEvent(trade::Order* rtnOrder):CSgOrderEvent(ORDER_EVENT_PARTIALLY_FILLED, rtnOrder){}
	};

	class SubmitFailedEvent : public CSgOrderEvent
	{
	public:
		SubmitFailedEvent(const string& errMsg):
		  CSgOrderEvent(ORDER_EVENT_SUBMIT_FAILED, NULL),
			  m_errMsg(errMsg){}

		  virtual const string& StatusMsg(){ return m_errMsg; }
	private:
		string m_errMsg;
	};
};



