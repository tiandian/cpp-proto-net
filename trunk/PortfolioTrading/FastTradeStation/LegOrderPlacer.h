#pragma once

#include "InputOrder.h"
#include "AsyncOrderPendingTimer.h"
#include "entity/trade.pb.h"
#include "entity/quote.pb.h"

#include <assert.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/chrono.hpp>

using namespace std;

class CPortfolioOrderPlacer;

class CLegOrderPlacer
{
public:
	CLegOrderPlacer(CPortfolioOrderPlacer* portfOrdPlacer, int openTimeout, int maxRetry);
	~CLegOrderPlacer(void);

	CInputOrder& InputOrder(){ return m_inputOrder; }
	trade::Order& Order(){ return m_legOrder; }
	
	int SequenceNo() { return m_seqenceNo; }
	void SequenceNo(int idx){ m_seqenceNo = idx; }

	int LegIndex(){ return m_legIdx; }
	void LegIndex(int legIdx){ m_legIdx = legIdx; }

	void OrderRef(const string& orderRef);
	const string& OrderRef();
	const string& Symbol(){ return m_inputOrder.Symbol(); }
	const string& ExchId(){ return m_exchId; }
	const string& OrderSysId(){ return m_ordSysId; }
	const string& UserId(){ return m_userId; }

	int SubmitTimes() { return m_submitTimes; }
	int AddSubmitTimes() { return ++m_submitTimes; }
	bool CanRetry(){ return m_submitTimes <= m_maxRetry; }
	void ModifyPrice(entity::Quote* pQuote);

	void StartPending(trade::Order* pendingOrder);

	bool IsOpen();
	void Reset();
	bool IsPending(){ return m_isPending; }

	double SetPriceTick(double pxTick){ m_priceTick = pxTick; }
	void UpdateOrder(const trade::Order& order);
	bool IsOrderReady(){ return m_bOrderReady; }

private:

	CInputOrder m_inputOrder;
	CPortfolioOrderPlacer* m_portfOrderPlacer;
	CAsyncOrderPendingTimer m_pendingTimer;
	
	int m_legIdx;
	int m_seqenceNo;
	int m_maxRetry;
	string m_currentOrdRef;
	int m_submitTimes;
	int m_openTimeout;
	double m_priceTick;

	trade::Order m_legOrder;
	bool m_bOrderReady;

	// pending Info;
	bool m_isPending;
	string m_exchId;
	string m_ordSysId; 
	string m_userId;
};

typedef boost::shared_ptr<CLegOrderPlacer> LegOrderPlacerPtr;

