#pragma once

#include "TradeAgentCallback.h"
#include "PortfolioOrderHelper.h"
#include "entity/quote.pb.h"

#ifndef USE_FEMAS_API
#include "RtnOrderWrapper.h"
#else
#include "RtnOrderWrapper_FM.h"
#endif // !USE_FEMAS_API

#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>

class CPortfolio;
class CTradeAgent;
class CAvatarClient;
class CInputOrder;
class CPortfolioOrderPlacer;

using namespace std;

class COrderProcessor : public CTradeAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void Initialize(CAvatarClient* pClientAgent, CTradeAgent* pTradeAgent);

	void AddOrderPlacer(CPortfolioOrderPlacer* pOrdPlacer);
	void RemoveOrderPlacer(const string& placerId /* order ref*/);
	
	int LockForSubmit(string& outOrdRef);
	bool SubmitAndUnlock(CInputOrder* pInputOrder);
	int GenerateOrderRef(string& outOrdRef);

	void CancelOrder(const string& ordRef, 
					 const string& exchId, 
					 const string& ordSysId, 
					 const string& userId,
					 const string& symbol);

	boost::tuple<bool, string> PlaceOrder( const string& symbol, 
										   trade::TradeDirectionType direction, 
										   const string& openDate, 
										   PlaceOrderContext* placeOrderCtx);

	bool QuerySymbol( const std::string& symbol, entity::Quote** ppQuote );

	bool QueryAccountInfo(string* outSerializedAcctInfo);
	void QueryPositionDetails(const string& symbol);

	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void PublishOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void PublishTradeUpdate(trade::Trade* pTrade);
	void PublishPositionDetail(trade::PositionDetailInfo* pPosiDetailInfo);
	
	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///登录请求响应
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///报单录入请求响应
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///报单操作请求响应
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, int errorId, const std::string& msg);

	///报单通知
	virtual void OnRtnOrder(RtnOrderWrapperPtr& orderWrapper);

	///成交通知
	virtual void OnRtnTrade(trade::Trade* pTrade);

	///请求查询投资者持仓明细响应
	virtual void OnRspQryInvestorPositionDetail(trade::PositionDetailInfo* pPositionDetail);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	virtual void OnRspQryInstrument(){}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo);

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

	bool IsReadyForSubmit(){ return !m_bIsSubmitting; }

	const string& BrokerId();
	const string& InvestorId();

private:

	static void PrintOrderStatus(trade::Order* order);
	
	void DispatchRtnOrder(RtnOrderWrapperPtr& orderWrapper);

	trade::InputOrder* BuildCloseOrder(const string& symbol, trade::TradeDirectionType direction, const string& openDate, PlaceOrderContext* placeOrderCtx);

	boost::unordered_map<string, CPortfolioOrderPlacer*> m_workingOrderPlacers;
	boost::recursive_mutex m_ordPlacersMapMutex;

	char m_orderRefBuf[13];
	int m_maxOrderRef;
	boost::mutex m_mutOrdRefIncr;
	boost::condition_variable m_condSubmit;
	bool m_bIsSubmitting;

	CTradeAgent* m_pTradeAgent;
	CAvatarClient* m_pClientAgent;

	boost::condition_variable m_condQryAcct;
	boost::mutex m_mutQryAcct;
	string m_serializedQryAcctInfo;
};

