#pragma once

#include "LegOrderPlacer.h"
#include "entity/trade.pb.h"
#include "entity/quote.pb.h"

#include <assert.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>

class CPortfolio;
class COrderProcessor2;
class CInputOrder;

using namespace std;

class CPortfolioOrderPlacer
{
public:
	CPortfolioOrderPlacer(void);
	~CPortfolioOrderPlacer(void);
	
	void Initialize(CPortfolio* pPortf, COrderProcessor2* pOrderProc)
	{
		m_pPortf = pPortf;
		m_pOrderProcessor = pOrderProc;
	}

	// Prepare multileg order template and input orders for sending
	void Prepare();

	void Cleanup();

	// Truly submit order to trade agent
	void Run(trade::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize);
	void Run(trade::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, 
		boost::chrono::steady_clock::time_point& trigQuoteTimestamp);
	
	// Identification for order processor to lookup while order returned
	const string& Id() 
	{
		assert(m_activeOrdPlacer != NULL);
		return m_activeOrdPlacer->OrderRef(); 
	}

	boost::chrono::steady_clock::time_point& TrigTimestamp(){ return m_trigQuoteTimestamp; }

	bool IsWorking(){ return m_isWorking.load(boost::memory_order_consume); }

	// For Fsm to begin sending leg order
	void OnSend();
	// On leg order submit done
	void OnAccept(trade::Order* pRtnOrder);
	
	void OnCanceling();
	virtual void OnLegCanceled(trade::Order* pRtnOrder);
	void OnLegRejected(trade::Order* pRtnOrder);
	void OnPortfolioCanceled();

	void OnPending(trade::Order* pRtnOrder);
	void OnPendingTimeUp();
	void OnQuoteReceived(boost::chrono::steady_clock::time_point& quoteTimestamp, entity::Quote* pQuote);

	void OnFilled(trade::Order* pRtnOrder);
	void OnPartiallyFilled(trade::Order* pRtnOrder);
	
	void OnCompleted();
	void OnError(const string& errMsg);

	// Belows is for OrderProcess to invoke
	void OnOrderReturned(boost::shared_ptr<trade::Order>& pRtnOrder);
	void OnOrderPlaceFailed(const string& errMsg);
	void OnOrderCancelFailed(const string& errMsg);

	bool IsActiveFirstLeg();
	void UpdateLegOrder(trade::Order* pRtnOrder);

protected:

	virtual void BuildTemplateOrder(){}
	virtual void Initialize(const string& mlOrdId);
	virtual void SetDirection(trade::PosiDirectionType posiDirection){}
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize){}
	
	void AfterLegDone();
	void AfterPortfolioDone();
	
	void GenLegOrderPlacers();
	
	void UpdateMultiLegOrder();
	void OutputStatus(const string& statusMsg);
	void CleanupProc();

	struct NextQuote
	{
		double last;
		double ask;
		double bid;
	};
	NextQuote m_nextQuote;

	CPortfolio* m_pPortf;
	COrderProcessor2* m_pOrderProcessor;

	boost::shared_ptr<trade::MultiLegOrder> m_multiLegOrderTemplate;
	vector< LegOrderPlacerPtr > m_legPlacers;
	CLegOrderPlacer* m_activeOrdPlacer;
	bool m_isFirstLeg;
	boost::atomic<bool> m_isWorking;
	bool m_isReady;
	boost::chrono::steady_clock::time_point m_trigQuoteTimestamp;

	boost::thread m_thCleanup;

	boost::shared_ptr<void> m_fsm;
	boost::mutex m_mutOuterAccessFsm;
};

