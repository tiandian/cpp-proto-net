#pragma once

#include "OrderState.h"
#include "OrderStateMachine.h"
#include "../Entity/gen/cpp/trade.pb.h"

class CSgOrderStateMachine;
class COrderProcessor2;

typedef boost::shared_ptr<trade::InputOrder> InputOrderPtr;

class CSgOrderPlacer : public COrderPlacer
{
public:
	CSgOrderPlacer(CSgOrderStateMachine* pStateMachine,
					trade::MultiLegOrder* pMultiLegOrder,
					const InputOrderPtr& inputOrder,
					int retryTimes,
					COrderProcessor2* pOrderProc)
					:m_pStateMachine(pStateMachine),
					m_pMultiLegOrder(pMultiLegOrder),
					m_pInputOrder(inputOrder),
					m_retryTimes(retryTimes),
					m_pOrderProcessor(pOrderProc){}
	~CSgOrderPlacer(){}

	const string& Id(){ return m_orderRef; }

	void Do();

private:
	string m_orderRef;
	CSgOrderStateMachine* m_pStateMachine;
	trade::MultiLegOrder* m_pMultiLegOrder;
	InputOrderPtr m_pInputOrder;
	COrderProcessor2* m_pOrderProcessor;
	
	int m_retryTimes;
};

class CSgOrderStateMachine : public COrderStateMachine
{
public:
	CSgOrderStateMachine(void);
	~CSgOrderStateMachine(void);

	OrderPlacerPtr CreatePlacer(trade::MultiLegOrder* pMultiLegOrder,
								const InputOrderPtr& pInputOrder,
								int retryTimes,
								COrderProcessor2* pOrderProc)
	{
		return OrderPlacerPtr(new CSgOrderPlacer(this, pMultiLegOrder, pInputOrder, retryTimes, pOrderProc));
	}

	void Init();
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



