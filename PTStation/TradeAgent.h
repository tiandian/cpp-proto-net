#pragma once

#include "ThostTraderApi/ThostFtdcTraderApi.h"
#include "TradeAgentCallback.h"
#include "SyncRequest.h"
#include "SymbolInfo.h"
#include "entity/quote.pb.h"

#ifdef FAKE_DEAL
#include "FakeDealer.h"
#endif

#include <boost/tuple/tuple.hpp>
#include <boost/date_time.hpp>
#include <boost/atomic.hpp>

class CTradeAgent : public CThostFtdcTraderSpi
{
public:
	CTradeAgent(void);
	~CTradeAgent(void);

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& userId, const string& password);
	void Logout();

	void SetCallbackHanlder(CTradeAgentCallback* pCallback);

	bool SubmitOrder(trade::InputOrder* pInputOrder);
	bool SubmitOrder(CThostFtdcInputOrderField& inputOrderField);
	bool SubmitOrderAction(trade::InputOrderAction* pInputOrderAction);

	void QueryAccount();
	void QueryOrders(const string& symbol);
	void QueryPositions();
	void QueryPositionDetails(const string& symbol);

	bool QuerySymbol(const string& symbol, entity::Quote** ppQuote);
	bool QuerySymbolAsync(const string& symbol, int nReqestId);
	bool QuerySymbolInfo(const string& symbol, CSymbolInfo** ppSymbolInfo);
	bool QuerySymbolInfoAsync( CSymbolInfo* pSymbolInfo, int nReqestId );

	const string& BrokerId(){ return m_brokerId; }
	const string& InvestorId(){ return m_investorId; }
	const boost::gregorian::date& TradingDay(){ return m_tradingDay; }

	//////////////////////////////////////////////////////////////////////////
	// Response trading related api

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询投资者持仓明细响应
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	virtual void OnHeartBeatWarning(int nTimeLapse){}

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///请求查询行情响应
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//////////////////////////////////////////////////////////////////////////

private:

	void RunTradingFunc(string address);
	void Login();
	void ReqSettlementInfoConfirm();
	int RequestIDIncrement() { return ++m_iRequestID; }
	bool IsMyOrder(CThostFtdcOrderField *pOrder)
	{ 
		return pOrder->FrontID == FRONT_ID && pOrder->SessionID == SESSION_ID;
	}

	string m_brokerId;
	string m_investorId;
	string m_password;

	// 请求编号
	boost::atomic<int> m_iRequestID;

	// 会话参数
	TThostFtdcFrontIDType	FRONT_ID;	//前置编号
	TThostFtdcSessionIDType	SESSION_ID;	//会话编号
	int m_maxOrderRef;					//报单引用
	boost::gregorian::date m_tradingDay;
	
	bool m_isConnected;
	bool m_isLogged;
	bool m_isConfirmed;
	string m_loginErr;

	boost::thread m_thTrading;
	bool m_isWorking;
	CThostFtdcTraderApi* m_pUserApi;

	boost::mutex m_mutConnecting;
	boost::condition_variable m_condConnecting;
	boost::mutex m_mutLogin;
	boost::condition_variable m_condLogin;
	boost::mutex m_mutConfirm;
	boost::condition_variable m_condConfirm;

	CSyncRequestFactory<entity::Quote> m_requestFactory;
	CSyncRequestFactory<CSymbolInfo> m_symbInfoReqFactory;

	CTradeAgentCallback* m_orderProcessor;
#ifdef FAKE_DEAL
	CFakeDealer m_fakeDealer;
#endif

};

