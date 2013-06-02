#include "StdAfx.h"
#include "LogManager.h"
#include "Configuration.h"

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>

#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/log/attributes/timer.hpp>

#include "LogSeverityLevel.h"

namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;
using namespace std;

// Now, let's try logging with severity
src::severity_logger< severity_level > slg;

extern CConfiguration config;

CLogManager::CLogManager(void):
	m_isEnabled(false)
{
}

CLogManager::~CLogManager(void)
{
}

void CLogManager::Init()
{
	bool enabled = config.IsLogEnabled();
	if(!enabled){
		cout << "Log is DISABLED" << endl;
		return;
	}

	try
	{
		// The first thing we have to do to get using the library is
		// to set up the logging sinks - i.e. where the logs will be written to.
		logging::init_log_to_console(std::clog, keywords::format = "%TimeStamp%: %_%");

		// One can also use lambda expressions to setup filters and formatters
		logging::init_log_to_file
			(
			config.GetLogFilePath(),
			keywords::filter = flt::attr< severity_level >("Severity", std::nothrow) >= config.GetLogLevel(),
			keywords::format = fmt::format("%1% [%2%] <%3%> %4%")
			% fmt::date_time("TimeStamp", std::nothrow)
			% fmt::time_duration("Uptime", std::nothrow)
			% fmt::attr< severity_level >("Severity", std::nothrow)
			% fmt::message()
			);

		// Also let's add some commonly used attributes, like timestamp and record counter.
		logging::add_common_attributes();

		// Let's pretend we also want to profile our code, so add a special timer attribute.
		slg.add_attribute("Uptime", boost::make_shared< attrs::timer >());

		m_isEnabled = true;
		cout << "Log initialized successfully" << endl;
	}
	catch(exception& e)
	{
		cerr << "Failed to initialize log due to " << e.what() << endl;
	}
}

void CLogManager::Trace( const char* text )
{
	if(m_isEnabled)
	{
		BOOST_LOG_SEV(slg, trace) << text;
	}
}

void CLogManager::Trace( std::string& text)
{
	Trace(text.c_str());
}

void CLogManager::Debug( const char* text )
{
	if(m_isEnabled)
	{
		BOOST_LOG_SEV(slg, debug) << text;
	}
}

void CLogManager::Debug( std::string& text)
{
	Debug(text.c_str());
}

void CLogManager::Info( const char* text )
{
	if(m_isEnabled)
	{
		BOOST_LOG_SEV(slg, info) << text;
	}
}

void CLogManager::Info( std::string& text)
{
	Info(text.c_str());
}

void CLogManager::Warning( const char* text )
{
	if(m_isEnabled)
	{
		BOOST_LOG_SEV(slg, warning) << text;
	}
}

void CLogManager::Warning( std::string& text)
{
	Warning(text.c_str());
}

void CLogManager::Error( const char* text )
{
	if(m_isEnabled)
	{
		BOOST_LOG_SEV(slg, error) << text;
	}
}

void CLogManager::Error( std::string& text)
{
	Error(text.c_str());
}

void CLogManager::Fatal( const char* text )
{
	if(m_isEnabled)
	{
		BOOST_LOG_SEV(slg, fatal) << text;
	}
}

void CLogManager::Fatal( std::string& text)
{
	Fatal(text.c_str());
}