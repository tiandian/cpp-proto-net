#pragma once

#include <string>

using namespace std;

class CConfiguration
{
public:
	CConfiguration(void);
	~CConfiguration(void);

	bool Load(int argc, char* argv[]);

	bool IsLogEnabled();
	const string& LogConfigPath();

	const string& GetPort();
	const string& GetAddr();

private:
	// Logging
	bool m_enableLogging;
	std::string m_logConfigFilePath;

	// Connection
	std::string m_addr;
	std::string m_port;

};

