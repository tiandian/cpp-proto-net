#pragma once
class CLogManager
{
public:
	CLogManager(void);
	~CLogManager(void);

	void Trace(char* text);
	void Debug(char* text);
	void Info(char* text);
	void Warning(char* text);
	void Error(char* text);
	void Fatal(char* text);

	void Init();

private:
	
	bool m_isEnabled;
};

