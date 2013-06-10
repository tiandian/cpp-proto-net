#pragma once

class CAvatarClient : public LogicalConnection
{
public:
	CAvatarClient(const string& sessionId);
	~CAvatarClient(void);

	virtual const char* getKey(){ return m_sessionId.c_str(); }

private:
	string m_sessionId;
};

