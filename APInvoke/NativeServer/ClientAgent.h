#pragma once

#include "../APInvokeNative/APInvokeNative.h"

class ClientAgent : public SessionCallback
{
public:
	ClientAgent(void);
	~ClientAgent(void);

	virtual void OnCallbackResponse(const string& method, const string& return_data);

	void SetSession(Session* session)
	{
		m_session = session;
	}

	void DoCallback();

private:
	Session* m_session;
};

