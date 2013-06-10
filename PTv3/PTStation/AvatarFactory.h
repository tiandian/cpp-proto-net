#pragma once

class CAvatarConnectionCxt : public ConnectionContext
{
public:
	CAvatarConnectionCxt()
	{
		//
	}
	~CAvatarConnectionCxt()
	{
		//
	}
	
	string puzzle;
};

class CAvatarFactory : public ClientFactory
{
public:
	CAvatarFactory(void);
	~CAvatarFactory(void);

protected:
	virtual OutgoingPacket* onNewConnection(ConnectionContext*& lpContext);
	virtual void onClientDisconnected(LogicalConnection* pClient, CloseReason closeReason);
	virtual void onClientConnected(LogicalConnection* pClient);
	virtual int onFirstRequest(IncomingPacket& request, ConnectionContext* lpContext, LogicalConnection*& lpClient, OutgoingPacket*& lpPacket);
};

