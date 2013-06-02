#pragma once

#include "BufferRunner.h"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

using namespace std;

enum TD_MSG_TYPE
{
	RSP_LOGIN,
	RSP_ACCOUNT,
	RSP_ORDER_INSERT,
	RTN_ORDER,
	RTN_TRADE,
	RSP_POSITION
};

class CTradeMessagePacket
{
public:
	CTradeMessagePacket(void* pData, TD_MSG_TYPE msgType)
	{
		m_pData = pData;
		m_msgType = msgType;
	}

	TD_MSG_TYPE MsgType() { return m_msgType; }
	void* Data() { return m_pData; }

private:
	TD_MSG_TYPE m_msgType;
	void* m_pData;
};

typedef boost::shared_ptr<CTradeMessagePacket> MessagePacketPtr;

class CTradeAgentCallback;

class CTradeMessagePump
{
public:
	CTradeMessagePump(void);
	~CTradeMessagePump(void);

	void Init(CTradeAgentCallback* pOrdProcessor);
	void Enqueue(MessagePacketPtr& msgPacket);
	void DispatchMessage(MessagePacketPtr& message);

private:
	CBufferRunner<MessagePacketPtr> m_bufferRunner;
	CTradeAgentCallback* m_orderProcessor;
};

class CRspUserLogin
{
public:
	bool LoginSuccess;
	string ErrorMsg;
	int InitOrderRef;
};

class CRspOrderInsert
{
public:
	bool InsertSucess;
	string OrderRef;
	string ErrorMsg;
};
