#pragma once

#include <string>
#include <boost/tuple/tuple.hpp>

using namespace std;

class CTradeAgent
{
public:
	CTradeAgent(void);
	~CTradeAgent(void);

	boost::tuple<bool, string> Open( const string& address, const string& streamDir );
	void Close();

	boost::tuple<bool, string> Login(const string& brokerId, const string& userId, const string& password);
	void Logout();

private:

};

