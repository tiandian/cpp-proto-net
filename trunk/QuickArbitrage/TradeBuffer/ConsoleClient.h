#pragma once
#include "clientbase.h"

#include <sstream>

class CConsoleClient :
	public ClientBase
{
public:
	CConsoleClient(void);
	~CConsoleClient(void);

	bool Login(const std::string& brokerId, const std::string& userId, const std::string& password);
	void Logout();

	void OnRegisterResult(bool succ, std::string& errMsg);

	void Buy();

protected:
	virtual void ProcessQuote(CTP::Quote* pQuote);

private:
	std::stringstream m_streamOut;

	ClientBase* m_pRemoteClient;
};

