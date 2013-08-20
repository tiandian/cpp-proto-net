#pragma once

#include "InputOrder.h"
#include "AsyncOrderPendingTimer.h"
#include "RtnOrderWrapper.h"
#include "entity/trade.pb.h"
#include "entity/quote.pb.h"

class CPortfolioOrderPlacer;

class CLegOrderPlacer
{
public:
	CLegOrderPlacer(CPortfolioOrderPlacer* portfOrdPlacer, int openTimeout, int maxRetry);
	~CLegOrderPlacer(void);

	CInputOrder& InputOrder(){ return m_inputOrder; }
	trade::Order& Order(){ return m_legOrder; }
	trade::Order& OrderEntity()
	{
		m_legOrderWrapper->ToEntity(&m_legOrder);
		return m_legOrder; 
	}
	
	unsigned int SequenceNo() { return m_sequenceNo; }
	void SequenceNo(unsigned int idx){ m_sequenceNo = idx; }

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
	bool ModifyPrice(entity::Quote* pQuote);

	void StartPending(const RtnOrderWrapperPtr& pendingOrder);

	bool IsOpen();
	void Reset(bool afterCancel = false);
	bool IsPending(){ return m_isPending; }
	void CancelPending();

	void SetPriceTick(double pxTick){ m_priceTick = pxTick; }
	void UpdateOrder(const RtnOrderWrapperPtr& order);
	bool IsOrderReady(){ return m_bOrderReady; }

	void PartiallyFill(int tradedCount) { m_isPartiallyFilled = true; }
	bool IsPartiallyFilled(){ return m_isPartiallyFilled; }
	void SetVolumeOriginal(int volOrig) { m_volumeOriginial = volOrig; }
	void AdjustVolume(int adjustedVolume);
private:

	CInputOrder m_inputOrder;
	CPortfolioOrderPlacer* m_portfOrderPlacer;
	CAsyncOrderPendingTimer m_pendingTimer;
	
	int m_legIdx;
	unsigned int m_sequenceNo;
	int m_maxRetry;
	string m_currentOrdRef;
	int m_submitTimes;
	int m_openTimeout;
	double m_priceTick;

	trade::Order m_legOrder;
	RtnOrderWrapperPtr m_legOrderWrapper;
	bool m_bOrderReady;

	// pending Info;
	bool m_isPending;
	string m_exchId;
	string m_ordSysId; 
	string m_userId;

	// partially filled
	bool m_isPartiallyFilled;
	int m_volumeOriginial;
};

typedef boost::shared_ptr<CLegOrderPlacer> LegOrderPlacerPtr;

