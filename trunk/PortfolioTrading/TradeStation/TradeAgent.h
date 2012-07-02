#pragma once

#include <string>

using namespace std;

class CTradeAgent
{
public:
	CTradeAgent(void);
	~CTradeAgent(void);

	bool Open(const string& address);
	void Close();

	bool Login(const string& brokerId, const string& userId, const string& password);
	void Logout();

private:

};

