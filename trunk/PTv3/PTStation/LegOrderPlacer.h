#pragma once

#include "AsyncOrderPendingTimer.h"

#include "entity/trade.pb.h"
#include "entity/quote.pb.h"

#ifndef USE_FEMAS_API
#include "InputOrder.h"
#include "RtnOrderWrapper.h"
#else
#include "InputOrder_FM.h"
#include "RtnOrderWrapper_FM.h"
#endif // !USE_FEMAS_API

class CPortfolioOrderPlacer;

enum MODIFY_PRICE_WAY
{
	BASED_ON_TICK,
	BASED_ON_OPPOSITE
};

class CLegOrderPlacer
{
public:
	CLegOrderPlacer(CPortfolioOrderPlacer* portfOrdPlacer, int openTimeout, int maxRetry);
	virtual ~CLegOrderPlacer(void);

	CInputOrder& InputOrder(){ return m_inputOrder; }
	trade::Order& Order(){ return m_legOrder; }
	trade::Order& OrderEntity();
	
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
	bool ModifyPriceBasedOnTick(entity::Quote* pQuote);
	bool ModifyPriceBasedOnOpposite(entity::Quote* pQuote);

	MODIFY_PRICE_WAY ModifyPriceWay(){ return m_modifyPriceWay; }
	void ModifyPriceWay(MODIFY_PRICE_WAY way){ m_modifyPriceWay = way; }

	void StartPending(const RtnOrderWrapperPtr& pendingOrder);

	virtual bool IsOpen();
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

	// OrderEntity status would be set after operation Reset because of Filled Order incoming after Reset 
	// So do this after portfolio completely done for ensuring correct status
	void ResetOrderEntityStatus(){ m_bOrderReady = false; }

protected:

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
	MODIFY_PRICE_WAY m_modifyPriceWay;

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

class CArbitrageLegOrderPlacer : public CLegOrderPlacer
{
public:
	CArbitrageLegOrderPlacer(CPortfolioOrderPlacer* portfOrdPlacer, int openTimeout, int maxRetry)
		: CLegOrderPlacer(portfOrdPlacer, openTimeout, maxRetry)
	{
		m_modifyPriceWay = BASED_ON_OPPOSITE;
	}
	~CArbitrageLegOrderPlacer(void){}

	bool IsOpen();
};

typedef boost::shared_ptr<CLegOrderPlacer> LegOrderPlacerPtr;



