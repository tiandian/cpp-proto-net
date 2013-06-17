#pragma once

class CAvatarClient : public LogicalConnection
{
public:
	CAvatarClient(const string& sessionId);
	~CAvatarClient(void);

	virtual const char* getKey(){ return m_sessionId.c_str(); }

	const string& Pseudo() const { return m_pseudo; }
	void Pseudo(const string& val) { m_pseudo = val; }

private:
	string m_sessionId;
	string m_pseudo;
	
};

