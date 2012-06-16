#pragma once

#include "../APInvokeNative/APInvokeNative.h"

class ServerSessionCallback : public SessionManagerHandler
{
public:
	ServerSessionCallback(void);
	~ServerSessionCallback(void);

	virtual void OnConnected(Session* session);

	virtual void OnDisconnected(Session* session);

	virtual void OnError(Session* session, const string& errorMsg);

	virtual void DispatchPacket(const string& sessionId, 
		const string& method, const string& in_data, string& out_data);
};

