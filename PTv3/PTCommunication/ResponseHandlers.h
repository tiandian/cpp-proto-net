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
