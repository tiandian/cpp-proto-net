#pragma once

#include "OrderStateMachine.h"
#include "entity/trade.pb.h"

#include <vector>
#include <boost/shared_ptr.hpp>

class CPortfolio;
class COrderProcessor2;
class CInputOrder;
class CInputOrderPlacer;

using namespace std;

class CPortfolioOrderPlacer : public COrderPlacer
{
public:
	CPortfolioOrderPlacer(CPortfolio* pPortf, COrderProcessor2* pOrderProc);
	~CPortfolioOrderPlacer(void);

	void Prepare();
	
	virtual const string& Id() { return m_isWorking ? m_multiLegOrderTemplate->orderid() : false; }
	virtual void Do();
	bool OnEnter(ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState);

	void Run(trade::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize);

protected:

	void Send();
	virtual bool SendNext(COrderEvent* transEvent);

	virtual void SetMlOrderId(const string& mlOrdId);
	virtual void SetDirection(trade::PosiDirectionType posiDirection){}
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize){}
	virtual void BuildTemplateOrder(){}

	int GetInputOrders(vector<boost::shared_ptr<CInputOrder> > * genInputOrders);
	CInputOrderPlacer* CreateInputOrderPlacer(const boost::shared_ptr<CInputOrder>& inputOrder, int retryTimes);
	void UpdateMultiLegOrder();
	void OutputStatus(const string& statusMsg);

	CPortfolio* m_pPortf;
	COrderProcessor2* m_pOrderProcessor;

	boost::shared_ptr<trade::MultiLegOrder> m_multiLegOrderTemplate;
	vector<boost::shared_ptr<CInputOrderPlacer> > m_inputOrderPlacers;

	bool m_isWorking;
	bool m_isSequential;
	int m_sendingIdx;
};

