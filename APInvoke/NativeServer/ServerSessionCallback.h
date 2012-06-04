#pragma once

#include "../APInvokeNative/APInvokeNative.h"

class ServerSessionCallback : public SessionManagerHandler
{
public:
	ServerSessionCallback(void);
	~ServerSessionCallback(void);

	virtual void OnConnected(const Session* session);

	virtual void OnDisconnected(const Session* session){}

	virtual void OnError(const Session* session, const string& errorMsg){}
};

