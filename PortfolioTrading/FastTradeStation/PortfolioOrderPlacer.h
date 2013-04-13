#pragma once

#include "AsyncScalperEventFirer.h"
#include "entity/trade.pb.h"
#include "entity/quote.pb.h"

#include <assert.h>
#include <vector>
#include <boost/shared_ptr.hpp>

class CPortfolio;
class COrderProcessor2;
class CInputOrder;

using namespace std;

class CPortfolioOrderPlacer
{
public:
	CPortfolioOrderPlacer(CPortfolio* pPortf, COrderProcessor2* pOrderProc);
	~CPortfolioOrderPlacer(void);
	
	// Prepare multileg order template and input orders for sending
	void Prepare();

	// Truly submit order to trade agent
	void Run(trade::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize);
	
	// Identification for order processor to lookup while order returned
	const string& Id() 
	{
		assert(m_isWorking);
		assert(m_sendingOrderRef.length() > 0);
		
		return m_sendingOrderRef; 
	}

	// For Fsm to begin sending leg order
	void OnSend();
	// On leg order submit done
	void OnAccept(trade::Order* pRtnOrder);
	
	void OnCanceling();
	void OnLegCanceled(trade::Order* pRtnOrder);
	void OnPortfolioCanceled();

	void OnPending(trade::Order* pRtnOrder);
	void OnPendingTimeUp();
	void OnQuoteReceived(entity::Quote* pQuote);

	void OnFilled(trade::Order* pRtnOrder);
	void OnPartiallyFilled(trade::Order* pRtnOrder);
	
	void OnCompleted();
	void OnError(const string& errMsg);

	// Reset all member variables after leg order done, thus ready for next leg order
	void Reset();

	// Belows is for OrderProcess to invoke
	void OnOrderReturned(trade::Order* pRtnOrder);
	void OnOrderPlaceFailed(const string& errMsg);
	void OnOrderCancelFailed(const string& errMsg);

protected:

	virtual void BuildTemplateOrder(){}
	virtual void Initialize(const string& mlOrdId);
	virtual void SetDirection(trade::PosiDirectionType posiDirection){}
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize){}
	virtual void ModifyOrderPrice();

	void AfterLegDone(bool isCanceled = false);
	void AfterPortfolioDone();
	
	int GenInputOrders();
	trade::Order* GetOrderBySymbol(const string& symbol, trade::TradeDirectionType direction);
	void SetPendingOrderInfo(trade::Order* pRtnOrder);
	
	void UpdateMultiLegOrder();
	void UpdateLegOrder(trade::Order* pRtnOrder);
	void OutputStatus(const string& statusMsg);

	struct PendingOrderInfo
	{
		string ordRef;
		string exchId;
		string ordSysId; 
		string userId;
		string symbol;
		char offsetFlag;
	};
	PendingOrderInfo m_pendingOrderInfo;

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
	vector<boost::shared_ptr<CInputOrder> > m_inputOrders;

	bool m_isWorking;
	bool m_isReady;
	bool m_isSequential;
	bool m_isClosingOrder;

	int m_sendingIdx;
	string m_sendingOrderRef;
	int m_maxRetryTimes;
	int m_submitTimes;

	boost::shared_ptr<void> m_fsm;
	boost::shared_ptr<CAsyncScalperEventFirer> m_openOrderTimer;
};

