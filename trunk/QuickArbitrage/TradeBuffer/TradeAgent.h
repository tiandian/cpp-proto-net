#pragma once

#include "ThostFtdcTraderApi.h"
#include "protobuf_gen/trade.pb.h"

#include <string>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>

class COrderManager;

class CTradeAgent : public CThostFtdcTraderSpi
{
public:
	CTradeAgent(COrderManager* pOrderMgr);
	~CTradeAgent(void);

	void Login(const std::string& brokerId, const std::string& userId, const std::string& password);
	void Logout();

	boost::tuple<std::string&, std::string&, std::string&> GetCurrentUserInfo()
	{
		return boost::make_tuple(boost::ref(m_brokerId), boost::ref(m_userId), boost::ref(m_password));
	}

	const std::string& GetUserId() { return m_userId; }

	void Buy();

	bool SubmitOrder( const std::vector< boost::shared_ptr<protoc::InputOrder> >& orders);
	bool SubmitOrder( protoc::InputOrder* pOrder);

	bool SubmitOrderAction (protoc::InputOrderAction* pOrderAction);

	void QueryAccount();

	int GetSessionID(){ return SESSION_ID; }
	int GetFrontID(){ return FRONT_ID; }
	int GetLastMaxOrderRef(){ return m_maxOrderRef; }

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

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

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

	//////////////////////////////////////////////////////////////////////////

private:
	int RequestIDIncrement(){ return ++m_iRequestID; }
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);
	void ReqSettlementInfoConfirm();

	CThostFtdcTraderApi* m_pUserApi;
	boost::thread m_thTrading;

	std::string m_brokerId;
	std::string m_userId;
	std::string m_password;

	bool m_isConnected;
	// ������
	int m_iRequestID;

	COrderManager* m_pOrderMgr;

	boost::mutex m_mutex;
	boost::condition_variable m_condLogout;

	// �Ự����
	TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
	TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
	TThostFtdcOrderRefType	ORDER_REF;	//��������

	int m_maxOrderRef;
};
