#include "StdAfx.h"
#include "LogManager.h"
#include "Configuration.h"

#include <log4cpp/Portability.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

using namespace std;

extern CConfiguration config;

log4cpp::Category& GetRootLogger()
{
	return log4cpp::Category::getRoot();
}

std::string loggerName("logger");

log4cpp::Category& GetLogger()
{
	return log4cpp::Category::getInstance(loggerName);
}

CLogManager::CLogManager(void):
	m_isEnabled(false)
{
}

CLogManager::~CLogManager(void)
{
	log4cpp::Category::shutdown();
}

void CLogManager::Init()
{
	std::string configFilePath("./log4cpp.property");
	Init(configFilePath);
}

void CLogManager::Init(const std::string& configPath)
{
	bool enabled = config.IsLogEnabled();
	if(!enabled){
		cout << "Log is DISABLED" << endl;
		return;
	}

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

bool CLogManager::IsTraceEnabled()
{
	return IsDebugEnabled();
}

void CLogManager::Trace( const char* text )
{
	if(m_isEnabled)
	{
		Debug(text);
	}
}

void CLogManager::Trace( const std::string& text)
{
	Debug(text);
}

bool CLogManager::IsDebugEnabled()
{
	if(m_isEnabled)
		return GetLogger().isDebugEnabled() || GetRootLogger().isDebugEnabled();

	return false;
}

void CLogManager::Debug( const char* text )
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		if(logger.isDebugEnabled())
			logger.debug(text);
		else
			GetRootLogger().debug(text);
	}
}

void CLogManager::Debug( const std::string& text)
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		if(logger.isDebugEnabled())
			logger.debug(text);
		else
			GetRootLogger().debug(text);
	}
}

bool CLogManager::IsInfoEnabled()
{
	if(m_isEnabled)
		return GetLogger().isInfoEnabled() || GetRootLogger().isInfoEnabled();
	return false;
}

void CLogManager::Info( const char* text )
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		if(logger.isInfoEnabled())
			logger.info(text);
		else
			GetRootLogger().info(text);
	}
}

void CLogManager::Info( const std::string& text)
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		if(logger.isInfoEnabled())
			logger.info(text);
		else
			GetRootLogger().info(text);
	}
}

bool CLogManager::IsWarningEnabled()
{
	if(m_isEnabled)
		return GetLogger().isWarnEnabled() || GetRootLogger().isWarnEnabled();
	return false;
}

void CLogManager::Warning( const char* text )
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		if(logger.isWarnEnabled())
			logger.warn(text);
		else
			GetRootLogger().warn(text);
	}
}

void CLogManager::Warning( const std::string& text)
{
	if(m_isEnabled)
	{
		log4cpp::Category& logger = GetLogger();
		if(logger.isWarnEnabled())
			logger.warn(text);
		else
			GetRootLogger().warn(text);
	}
}

bool CLogManager::IsErrorEnabled()
{
	if(m_isEnabled)
		return GetLogger().isErrorEnabled() || GetRootLogger().isErrorEnabled();
	return false;
}

void CLogManager::Error( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().error(text);
	}
}

void CLogManager::Error( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().error(text);
	}
}

bool CLogManager::IsFatalEnabled()
{
	if(m_isEnabled)
		return GetLogger().isFatalEnabled() || GetRootLogger().isFatalEnabled();
	return false;
}

void CLogManager::Fatal( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().fatal(text);
	}
}

void CLogManager::Fatal( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().fatal(text);
	}
}