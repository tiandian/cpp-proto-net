#pragma once

#include <string>

class CLogManager
{
public:
	CLogManager(void);
	~CLogManager(void);

	void Trace(const char* text);
	void Trace(const std::string& text);
	void Debug(const char* text);
	void Debug(const std::string& text);
	void Info(const char* text);
	void Info(const std::string& text);
	void Warning(const char* text);
	void Warning(const std::string& text);
	void Error(const char* text);
	void Error(const std::string& text);
	void Fatal(const char* text);
	void Fatal(const std::string& text);

	void Init();

private:
	
	bool m_isEnabled;
};

