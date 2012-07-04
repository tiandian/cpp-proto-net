#pragma once

#include <string>

#include "LogSeverityLevel.h"

class CConfiguration
{
public:
	CConfiguration(void);
	~CConfiguration(void);

	bool Load(int argc, _TCHAR* argv[]);

	bool IsLogEnabled();
	const char* GetLogFilePath();
	severity_level GetLogLevel();

	unsigned short GetPort();

private:
	// Logging
	bool m_enableLogging;
	std::string m_logFilePath;
	severity_level m_logLevel;

	// Connection
	unsigned short m_port;

};

