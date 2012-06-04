#pragma once

#include "APInvokeNative.h"
#include "server.h"

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>

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
	void OnClientAccepted(connection_ptr conn);

	SessionManagerHandler* m_callback;

	boost::shared_ptr<server> m_server;
};

