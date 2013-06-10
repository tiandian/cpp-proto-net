#pragma once

#define APP_VERSION "2.0.1"

class CConfiguration
{
public:
	CConfiguration(void);
	~CConfiguration(void);

	bool Load(int argc, char* argv[]);

	bool IsLogEnabled();
	const string& LogConfigPath();

	const string& GetPort();
	int GetPortNum();
	const string& GetAddr();

private:
	// Logging
	bool m_enableLogging;
	std::string m_logConfigFilePath;

	// Connection
	std::string m_addr;
	std::string m_port;

};

