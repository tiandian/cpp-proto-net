#pragma once

#include <string>

using namespace std;

class SessionCallback
{
public:
	virtual ~SessionCallback(){}

	virtual void OnCallbackResponse(const string& method, const string& return_data){}
};

class Session
{
public:
	virtual ~Session(void){}

	virtual const string& SessionId() = 0;

	virtual bool IsConnected() = 0;

	virtual void BeginCallback(const string& method, const string& callbackReqData) = 0;

	virtual void RegisterCallback(SessionCallback* callbackRsp) = 0;
};

class SessionManagerHandler
{
public:

	virtual bool OnConnected(Session* session, const string& clientId, bool attach){ return false; }

	virtual void OnDisconnected(Session* session){}

	virtual bool VerifyClient(const string& username, const string& password, bool* clientExisting){ return true; }

	virtual void OnError(Session* session, const string& errorMsg){}

	virtual void DispatchPacket(const string& sessionId, 
		const string& method, const string& in_data, string& out_data){}
};

class SessionManager
{
public:
	static SessionManager *Create();

	virtual ~SessionManager(void){}

	virtual void RegisterHandler(SessionManagerHandler* handler) = 0;

	virtual bool Listen(const string& sAddr, const string& sPort) = 0;

	virtual void Close() = 0;

};