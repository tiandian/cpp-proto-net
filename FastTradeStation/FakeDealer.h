#pragma once

#include "BufferRunner.h"
#include "ThostTraderApi/ThostFtdcTraderApi.h"

#include <string>
#include <sstream>
#include <boost/date_time.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>

using namespace std;

typedef boost::function< void(CThostFtdcOrderField *pOrder) > RtnOrderFunc;
typedef boost::function< void(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )> RspOrderInsertFunc;
typedef boost::function< void(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)> RspOrderActionFunc;

enum FakeMsgType { MSG_RTN_ORDER, MSG_RSP_ORDER_INSERT, MSG_RSP_ORDER_ACTION };

class CFakeMessage
{
public:
	virtual ~CFakeMessage(){}
	virtual FakeMsgType MsgType() = 0;
};

template<typename T>
class CFakeOrderMessage : public CFakeMessage
{
public:
	CFakeOrderMessage(int nReq):m_nReq(nReq)
	{
		memset(&m_msg, 0, sizeof(m_msg));
	}
	virtual ~CFakeOrderMessage(){}
	T* Msg(){ return &m_msg; }
	int RequestId() { return m_nReq; }
protected:
	T m_msg;
	int m_nReq;
};

class CFakeRtnOrder : public CFakeOrderMessage<CThostFtdcOrderField>
{
public:
	CFakeRtnOrder(int nReq):CFakeOrderMessage<CThostFtdcOrderField>(nReq){}
	FakeMsgType MsgType() { return MSG_RTN_ORDER; }
};
class CFakeRspOrderInsert : public CFakeOrderMessage<CThostFtdcInputOrderField>
{
public:
	CFakeRspOrderInsert(int nReq):CFakeOrderMessage<CThostFtdcInputOrderField>(nReq){}
	FakeMsgType MsgType() { return MSG_RSP_ORDER_INSERT; }
};
class CFakeRspOrderAction : public CFakeOrderMessage<CThostFtdcInputOrderActionField>
{
public:
	CFakeRspOrderAction(int nReq):CFakeOrderMessage<CThostFtdcInputOrderActionField>(nReq){}
	FakeMsgType MsgType() { return MSG_RSP_ORDER_ACTION; }
};

typedef boost::shared_ptr<CFakeMessage> FakeMsgPtr;

class CFakeDealer
{
public:
	CFakeDealer(void);
	~CFakeDealer(void);

	void Init(RtnOrderFunc funcRtnOrd, RspOrderInsertFunc funcRspOrdIns, RspOrderActionFunc funcRspOrdAct)
	{
		m_funcRtnOrder = funcRtnOrd;
		m_funcRspOrderInsert = funcRspOrdIns;
		m_funcRspOrderAction = funcRspOrdAct;
	}

	int ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder, int nRequestID);
	int ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);

	void DispatchMsg(FakeMsgPtr& fakeMsg);

	void SetSessionParams(int frontId, int sessionId)
	{
		FRONT_ID = frontId;
		SESSION_ID = sessionId;
	}

private:
	CFakeRtnOrder* CreateOrderTemplate(CThostFtdcInputOrderField * pInputOrder, int nRequestID);

	CFakeRtnOrder* GetAcceptOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID);
	CFakeRtnOrder* GetPendingOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId);
	CFakeRtnOrder* GetFilledOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId);
	CFakeRtnOrder* GetCanceledOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId);

	void FullFillOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID);
	void PendingOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID);
	void CancelOrder(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);
	
	void SetDateField(CThostFtdcOrderField* pRtnOrder);

	RtnOrderFunc m_funcRtnOrder;
	RspOrderInsertFunc m_funcRspOrderInsert;
	RspOrderActionFunc m_funcRspOrderAction;

	CBufferRunner<FakeMsgPtr> m_msgPump;
	boost::atomic<int> m_orderNum;

	// 会话参数
	TThostFtdcFrontIDType	FRONT_ID;	//前置编号
	TThostFtdcSessionIDType	SESSION_ID;	//会话编号

	string m_tradingDay;
	
	CThostFtdcInputOrderField m_pendingInputOrder;
	int m_pendingOrdSysId;

	std::ostringstream m_timeStream;
	//boost::mutex m_mutTimeFormat;
};

