#pragma once

#include "APInvokeNative.h"

#include <string> 

using namespace std;

class APSession : public Session
{
public:
	const string& SessionId() { return m_sessionId; }

private:
	string m_sessionId;
};

class APSessionManager : public SessionManager
{
public:
	APSessionManager(void);
	virtual ~APSessionManager(void);

	void RegisterCallback(SessionManagerHandler* handler);

	bool Listen(unsigned int nPort);

	void Close();

private:
	SessionManagerHandler* m_callback;
};

