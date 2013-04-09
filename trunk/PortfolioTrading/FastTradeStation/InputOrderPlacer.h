#pragma once

#include "OrderStateMachine.h"
#include "InputOrder.h"
#include "entity/trade.pb.h"

#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;

class CPortfolio;
class COrderProcessor2;

class CInputOrderPlacer : public COrderPlacer
{
public:
	CInputOrderPlacer(COrderStateMachine* pStateMachine,
		CPortfolio* pPortfolio,
		trade::MultiLegOrder* pMultiLegOrder,
		const boost::shared_ptr<CInputOrder>& inputOrder,
		int maxRetryTimes, bool allowPending,
		COrderProcessor2* pOrderProc)
		:m_pStateMachine(pStateMachine),
		m_pPortf(pPortfolio),
		m_pMultiLegOrder(pMultiLegOrder),
		m_pInputOrder(inputOrder),
		m_maxRetryTimes(maxRetryTimes),
		m_submitTimes(0), m_succ(false), 
		m_allowPending(allowPending),
		m_quoteTimestamp(0),
		m_pOrderProcessor(pOrderProc){}
	
	~CInputOrderPlacer(void){}


	const string& ParentOrderId(){ return m_pMultiLegOrder != NULL ? m_pMultiLegOrder->orderid() : EmptyParentOrderId; }
	const string& Symbol() { return m_pInputOrder->Symbol(); }
	const string& Id(){ return m_currentOrdRef; }
	const string& CompositeId() { return m_sgOrderUid; }

	void SetPrice(double price);
	void SetDirection(trade::TradeDirectionType direction);

	bool OnEnter(ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState);

	void Do();
	void AdjustQuantity(int qty);
	long QuoteTimestamp(){ return m_quoteTimestamp; }

protected:
	void CancelOrder(trade::Order* pOrd);

	virtual void RaiseMultiLegOrderEvent(COrderEvent* orderEvent);
	virtual void ModifyOrderPrice();

	virtual void OnSubmittingOrder();
	virtual void OnOrderUpdate(trade::Order* pOrd);
	virtual void OnOrderPlaceFailed(COrderEvent* pOrdEvent);
	virtual void OnOrderAccept(trade::Order* pOrd);
	virtual void OnPending(trade::Order* pOrd);
	virtual void OnCanceling(COrderEvent* transEvent);
	virtual void OnOrderComplete(trade::Order* pOrd);


	static string EmptyParentOrderId;

	string m_sgOrderUid;
	string m_currentOrdRef;
	COrderStateMachine* m_pStateMachine;
	trade::MultiLegOrder* m_pMultiLegOrder;
	CPortfolio* m_pPortf;
	boost::shared_ptr<CInputOrder> m_pInputOrder;
	COrderProcessor2* m_pOrderProcessor;

	int m_maxRetryTimes;
	int m_submitTimes;
	bool m_succ;
	string m_errorMsg;
	bool m_allowPending;

	long m_quoteTimestamp;

};

