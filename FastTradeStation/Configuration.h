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
	const string& GetWorkingDir();

private:
	// Logging
	bool m_enableLogging;
	string m_logConfigFilePath;

	// Connection
	string m_addr;
	string m_port;
	
	// Working directory
	string m_workingDir;
};

