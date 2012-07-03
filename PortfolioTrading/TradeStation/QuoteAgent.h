#pragma once

#include <string>
#include <vector>

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

	bool SubscribesQuotes( vector<string>& subscribeArr );
	bool UnSubscribesQuotes( vector<string>& unSubscribeArr );

};

