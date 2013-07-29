#pragma once

#include "TradeAgentCallback.h"
#include "ThostTraderApi/ThostFtdcTraderApi.h"
#include "SyncRequest.h"
#include "TradeMessagePump.h"
#include "../Entity/gen/cpp/quote.pb.h"
#include "SymbolInfo.h"

#include <string>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time.hpp>

using namespace std;

class CTradeAgent : public CThostFtdcTraderSpi
{
public:
	CTradeAgent(void);
	~CTradeAgent(void);

	boost::tuple<bool, string> Open( const string& address, const string& streamDir );
	void Close();

	boost::tuple<bool, string> Login(const string& brokerId, const string& userId, const string& password);
	void Logout();

	void SetCallbackHanlder(CTradeAgentCallback* pCallback);

	bool SubmitOrder(trade::InputOrder* pInputOrder);
	bool SubmitOrderAction(trade::InputOrderAction* pInputOrderAction);

	void ReqSettlementInfoConfirm();

	void QueryAccount();
	void QueryOrders(const std::string& symbol);
	void QueryPositions();
	void QueryPositionDetails(const std::string& symbol);

	bool QuerySymbol(const std::string& symbol, entity::Quote** ppQuote);
	bool QuerySymbolAsync(const std::string& symbol, int nReqestId);

	bool QuerySymbolInfo(const std::string& symbol, CSymbolInfo** ppSymbolInfo);
	bool QuerySymbolInfoAsync( CSymbolInfo* pSymbolInfo, int nReqestId );

	bool IsConnected(){ return m_bIsConnected; }
	bool IsDisconnected(){ return !m_bIsConnected; }

	const boost::gregorian::date& TradingDay(){ return m_tradingDay; }

	//////////////////////////////////////////////////////////////////////////
	// Response trading related api

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳���ϸ��Ӧ
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ���������
	virtual void OnFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	
	///�����ѯ������Ӧ
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);;

	//////////////////////////////////////////////////////////////////////////

private:
	int RequestIDIncrement();
	bool IsMyOrder(CThostFtdcOrderField *pOrder)
	{ 
		return pOrder->FrontID == FRONT_ID && pOrder->SessionID == SESSION_ID;
	}

	// ������
	int m_iRequestID;
	boost::mutex m_mutReqIdInc;

	boost::condition_variable m_condLogin;
	boost::mutex m_mutLogin;
	string m_sLoginError;
	bool m_loginSuccess;

	CThostFtdcTraderApi* m_pUserApi;
	
	bool m_bIsConnected;
	bool m_bIsConnecting;

	boost::condition_variable m_condConnectDone;
	boost::mutex m_mutex;
	boost::thread m_thQuoting;

	string m_brokerID;
	string m_userID;

	// �Ự����
	TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
	TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
	int m_maxOrderRef;					//��������
	boost::gregorian::date m_tradingDay;

	CSyncRequestFactory<entity::Quote> m_requestFactory;
	CTradeMessagePump m_messagePump;

	CSyncRequestFactory<CSymbolInfo> m_symbInfoReqFactory;
};
