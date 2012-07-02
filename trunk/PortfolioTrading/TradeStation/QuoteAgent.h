#pragma once

#include <string>

using namespace std;

class CQuoteAgent
{
public:
	CQuoteAgent(void);
	~CQuoteAgent(void);

	bool Open(const string& address);
	void Close();

	bool Login(const string& brokerId, const string& userId, const string& password);
	void Logout();

};

