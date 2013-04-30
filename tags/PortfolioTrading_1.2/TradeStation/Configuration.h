#pragma once

#include <string>

#include "LogSeverityLevel.h"

using namespace std;

class CConfiguration
{
public:
	CConfiguration(void);
	~CConfiguration(void);

	bool Load(int argc, _TCHAR* argv[]);

	bool IsLogEnabled();
	const char* GetLogFilePath();
	severity_level GetLogLevel();

	const string& GetPort();
	const string& GetAddr();

private:
	// Logging
	bool m_enableLogging;
	std::string m_logFilePath;
	severity_level m_logLevel;

	// Connection
	std::string m_addr;
	std::string m_port;

};

