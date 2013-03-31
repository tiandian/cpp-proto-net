#pragma once

#include <string>

#define LOG_DEBUG(lg, txt) LOG_WRITE(lg, txt, Debug)
#define LOG_INFO(lg, txt) LOG_WRITE(lg, txt, Info)
#define LOG_WRITE(lg, txt, lvl) if(lg.Is##lvl##Enabled()) lg.##lvl##(txt)

class CLogManager
{
public:
	CLogManager(void);
	~CLogManager(void);

	bool IsTraceEnabled();
	void Trace(const char* text);
	void Trace(const std::string& text);
	bool IsDebugEnabled();
	void Debug(const char* text);
	void Debug(const std::string& text);
	bool IsInfoEnabled();
	void Info(const char* text);
	void Info(const std::string& text);
	bool IsWarningEnabled();
	void Warning(const char* text);
	void Warning(const std::string& text);
	bool IsErrorEnabled();
	void Error(const char* text);
	void Error(const std::string& text);
	bool IsFatalEnabled();
	void Fatal(const char* text);
	void Fatal(const std::string& text);

	void Init();

private:
	
	bool m_isEnabled;
};

