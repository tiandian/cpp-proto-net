#pragma once

#include "LegOrderPlacer.h"
#include "RtnOrderWrapper.h"
#include "entity/trade.pb.h"
#include "entity/quote.pb.h"

#include <boost/atomic.hpp>

class CPortfolio;
class COrderProcessor;
class CInputOrder;

using namespace std;

class CPortfolioOrderPlacer
{
public:
	CPortfolioOrderPlacer(void);
	virtual ~CPortfolioOrderPlacer(void);
	
	void Initialize(CPortfolio* pPortf, COrderProcessor* pOrderProc)
	{
		m_pPortf = pPortf;
		m_pOrderProcessor = pOrderProc;
	}

	// Prepare multileg order template and input orders for sending
	void Prepare();

	void Cleanup();

	// Truly submit order to trade agent
	void Run(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize);
	void Run(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, 
		const boost::chrono::steady_clock::time_point& trigQuoteTimestamp);
	
	// Identification for order processor to lookup while order returned
	const string& Id() 
	{
		assert(m_activeOrdPlacer != NULL);
		return m_activeOrdPlacer->OrderRef(); 
	}

	boost::chrono::steady_clock::time_point& TrigTimestamp(){ return m_triggingTimestamp; }

	bool IsWorking(){ return m_isWorking.load(boost::memory_order_consume); }

	// For Fsm to begin sending leg order
	void Send();
	// On leg order submit done
	void OnAccept(const RtnOrderWrapperPtr& pRtnOrder);
	
	void OnCanceling();
	virtual void OnLegCanceled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnLegRejected(const RtnOrderWrapperPtr& pRtnOrder);
	void OnPortfolioCanceled();

	void OnPending(const RtnOrderWrapperPtr& pRtnOrder);
	void OnPendingTimeUp();
	void OnQuoteReceived(boost::chrono::steady_clock::time_point& quoteTimestamp, entity::Quote* pQuote);

	void OnFilled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnPartiallyFilled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnOrderCanceled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnCompleted();
	void OnError(const string& errMsg);
	void OnPartiallyFilledToCanceling(const RtnOrderWrapperPtr& pRtnOrder);

	// Belows is for OrderProcess to invoke
	void OnOrderReturned(RtnOrderWrapperPtr& rtnOrder);
	void OnOrderPlaceFailed(const string& errMsg);
	void OnOrderCancelFailed(int errorId, const string& errMsg);

	bool IfPortfolioCanceled();

	entity::PosiDirectionType PosiDirection(){ return m_posiDirection; }
	void OutputStatus(const string& statusMsg);
	void SetMlOrderStatus(const string& statusMsg);
protected:

	void Initialize(const string& mlOrdId);

	virtual void BuildTemplateOrder(){}
	virtual void SetDirection(entity::PosiDirectionType posiDirection);
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize);
	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer){}
	virtual void OnLegOrderSent(int orderPlacerIdx){}
	virtual void OnPortfolioDone(){}
	
	
	void SetNewOrderId(const string& mlOrdId);
	void ResetTemplate();
	void FillSendingOrderNote();
	void ResetSendingOrderNote();
	void SetFirstLeg();
	void GotoRetry(const RtnOrderWrapperPtr& pRtnOrder);
	void GotoNext();
	void AfterLegDone();
	void AfterPortfolioDone();
	
	void GenLegOrderPlacers();
	void UpdateLegOrder(const RtnOrderWrapperPtr& pRtnOrder);
	void UpdateLastDoneOrder();
	void UpdateMultiLegOrder();
	
	void CleanupProc();
	bool IsReadyForPrepare(){ return !m_isReady; }

	bool SendNextOnFilled(){ return m_sendNextOnFilled; }
	void SendNextOnFilled(bool val){ m_sendNextOnFilled = val; }

	struct NextQuote
	{
		double last;
		double ask;
		double bid;
	};
	NextQuote m_nextQuote;

	CPortfolio* m_pPortf;
	COrderProcessor* m_pOrderProcessor;

	boost::shared_ptr<trade::MultiLegOrder> m_multiLegOrderTemplate;
	vector< LegOrderPlacerPtr > m_legPlacers;
	CLegOrderPlacer* m_activeOrdPlacer;
	CLegOrderPlacer* m_lastDoneOrdPlacer;
	bool m_isFirstLeg;
	boost::atomic<bool> m_isWorking;
	bool m_isReady;
	bool m_sendNextOnFilled;
	string m_sendingOrderNote;
	entity::PosiDirectionType m_posiDirection;
	boost::chrono::steady_clock::time_point m_triggingTimestamp;

	boost::thread m_thCleanup;
	boost::condition_variable m_condCleanDone;
	boost::mutex m_mutCleaning;

	boost::shared_ptr<void> m_fsm;
	boost::mutex m_mutOuterAccessFsm;
};

typedef boost::shared_ptr<CPortfolioOrderPlacer> OrderPlacerPtr;