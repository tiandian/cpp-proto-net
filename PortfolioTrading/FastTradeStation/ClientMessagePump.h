#pragma once

#include "BufferRunner.h"
#include "ap_invoke/session.h"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

using namespace std;

class CClientMessagePacket
{
public:
	CClientMessagePacket(const char* cbName, string& cbData):
	  m_callbackName(cbName)
	  {
		  swap(m_callbackData, cbData);
	  }


	const string& MethodName() {return m_callbackName; }
	const string& Data(){ return m_callbackData; }

private:
	string m_callbackName;
	string m_callbackData;
};

typedef boost::shared_ptr<CClientMessagePacket> ClientMsgPacketPtr;

class CClientMessagePump
{
public:
	CClientMessagePump(void);
	~CClientMessagePump(void);

	void Init(Session* pSession);
	void Enqueue(ClientMsgPacketPtr& msgPacket);
	void DispatchMessage(ClientMsgPacketPtr& message);

private:
	CBufferRunner<ClientMsgPacketPtr> m_bufferRunner;
	Session* m_pSession;
};

