#include "ClientMessagePump.h"


CClientMessagePump::CClientMessagePump(void):
m_pSession(NULL),
m_bufferRunner(boost::bind(&CClientMessagePump::DispatchMessage, this, _1))
{
}


CClientMessagePump::~CClientMessagePump(void)
{
	m_bufferRunner.Stop();
}

void CClientMessagePump::Enqueue( ClientMsgPacketPtr& msgPacket )
{
	m_bufferRunner.Enqueue(msgPacket);
}

void CClientMessagePump::DispatchMessage( ClientMsgPacketPtr& message )
{
	if(m_pSession != NULL)
	{
		m_pSession->BeginCallback(message->MethodName(), message->Data());
	}
}

void CClientMessagePump::Init( Session* pSession )
{
	m_pSession = pSession;
	m_bufferRunner.Start();
}
