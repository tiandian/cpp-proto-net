#pragma once

#include <string>

using namespace std;

class CQSConfiguration
{
public:
	CQSConfiguration(void);
	~CQSConfiguration(void);

	bool Load(int argc, char* argv[]);

	const std::string& ConnectionString() const { return m_connectionString; }
	const std::string& BrokerId() const { return m_brokerId; }
	const std::string& Username() const { return m_username; }
	const std::string& Password() const { return m_password; }

	bool TestHost() { return m_testHost; }
	bool LogQuote() { return m_logQuote; }

private:
	
	string m_connectionString;
	string m_brokerId;
	string m_username;
	string m_password;

	bool m_testHost;
	bool m_logQuote;
};

