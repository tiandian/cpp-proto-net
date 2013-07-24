#pragma once

#include "ResponseHandler.h"

class CNatvieClient;

class SessionResponseHandler : public ResponseHandler
{
public:
	SessionResponseHandler(CNatvieClient* pSession) : m_pSession(pSession){}
	virtual ~SessionResponseHandler(){}
protected:
	CNatvieClient* m_pSession;
};

class LoginResponseHandler : public SessionResponseHandler
{
public:
	LoginResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~LoginResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);

};

class LoginPuzzleResponseHandler : public SessionResponseHandler
{
public:
	LoginPuzzleResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~LoginPuzzleResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);

};

class ServerLoginResponseHandler : public SessionResponseHandler
{
public:
	ServerLoginResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~ServerLoginResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);

};

class PortfolioUpdateResponseHandler : public SessionResponseHandler
{
public:
	PortfolioUpdateResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~PortfolioUpdateResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);
};

class MultilegOrderUpdateResponseHandler : public SessionResponseHandler
{
public:
	MultilegOrderUpdateResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~MultilegOrderUpdateResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);
};

class LegOrderUpdateResponseHandler : public SessionResponseHandler
{
public:
	LegOrderUpdateResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~LegOrderUpdateResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);
};

class TradeUpdateResponseHandler : public SessionResponseHandler
{
public:
	TradeUpdateResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~TradeUpdateResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);
};

class HeartbeatResponseHandler : public SessionResponseHandler
{
public:
	HeartbeatResponseHandler(CNatvieClient* pSession) : SessionResponseHandler(pSession)
	{}
	~HeartbeatResponseHandler(){}

	virtual void handleResponse(PushFramework::IncomingPacket& packet);
};
