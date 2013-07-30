#include "StdAfx.h"
#include "QS_LogManager.h"

#include <log4cpp/Portability.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

log4cpp::Category& GetLogger()
{
	return log4cpp::Category::getRoot();
}

CQSLogManager::CQSLogManager(void):
	m_isEnabled(false)
{
}

CQSLogManager::~CQSLogManager(void)
{
	log4cpp::Category::shutdown();
}

void CQSLogManager::Init()
{
	std::string configFilePath("./qs-log4cpp.property");
	Init(configFilePath);
}

void CQSLogManager::Init(const std::string& configPath)
{
	try
	{
		log4cpp::PropertyConfigurator::configure(configPath);
		m_isEnabled = true;
		cout << "Log initialized successfully" << endl;
	}
	catch(exception& e)
	{
		cerr << "Failed to initialize log due to " << e.what() << endl;
	}
}

bool CQSLogManager::IsTraceEnabled()
{
	return IsDebugEnabled();
}

void CQSLogManager::Trace( const char* text )
{
	if(m_isEnabled)
	{
		Debug(text);
	}
}

void CQSLogManager::Trace( const std::string& text)
{
	Debug(text);
}

bool CQSLogManager::IsDebugEnabled()
{
	if(m_isEnabled)
		return GetLogger().isDebugEnabled();

	return false;
}

void CQSLogManager::Debug( const char* text )
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		logger.debug(text);
	}
}

void CQSLogManager::Debug( const std::string& text)
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		logger.debug(text);
	}
}

bool CQSLogManager::IsInfoEnabled()
{
	if(m_isEnabled)
		return GetLogger().isInfoEnabled();
	return false;
}

void CQSLogManager::Info( const char* text )
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		logger.info(text);
	}
}

void CQSLogManager::Info( const std::string& text)
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		logger.info(text);
	}
}

bool CQSLogManager::IsWarningEnabled()
{
	if(m_isEnabled)
		return GetLogger().isWarnEnabled();
	return false;
}

void CQSLogManager::Warning( const char* text )
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		logger.warn(text);
	}
}

void CQSLogManager::Warning( const std::string& text)
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		logger.warn(text);
	}
}

bool CQSLogManager::IsErrorEnabled()
{
	if(m_isEnabled)
		return GetLogger().isErrorEnabled();
	return false;
}

void CQSLogManager::Error( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().error(text);
	}
}

void CQSLogManager::Error( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().error(text);
	}
}

bool CQSLogManager::IsFatalEnabled()
{
	if(m_isEnabled)
		return GetLogger().isFatalEnabled();
	return false;
}

void CQSLogManager::Fatal( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().fatal(text);
	}
}

void CQSLogManager::Fatal( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().fatal(text);
	}
}