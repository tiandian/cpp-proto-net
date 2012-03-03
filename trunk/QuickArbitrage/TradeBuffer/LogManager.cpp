#include "StdAfx.h"
#include "LogManager.h"

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

namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

// Here we define our application severity levels.
enum severity_level
{
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
	std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
{
	static const char* const str[] =
	{
		"trace",
		"debug",
		"info",
		"warning",
		"error",
		"fatal"
	};
	if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
		strm << str[lvl];
	else
		strm << static_cast< int >(lvl);
	return strm;
}

// Now, let's try logging with severity
src::severity_logger< severity_level > slg;

CLogManager::CLogManager(void)
{
}

CLogManager::~CLogManager(void)
{
}

void CLogManager::Init()
{
	// The first thing we have to do to get using the library is
	// to set up the logging sinks - i.e. where the logs will be written to.
	logging::init_log_to_console(std::clog, keywords::format = "%TimeStamp%: %_%");

	// One can also use lambda expressions to setup filters and formatters
	logging::init_log_to_file
	(
		"log.txt",
		keywords::filter = flt::attr< severity_level >("Severity", std::nothrow) >= warning,
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
}

void CLogManager::Trace( char* text )
{
	BOOST_LOG_SEV(slg, trace) << text;
}

void CLogManager::Debug( char* text )
{
	BOOST_LOG_SEV(slg, debug) << text;
}

void CLogManager::Info( char* text )
{
	BOOST_LOG_SEV(slg, info) << text;
}

void CLogManager::Warning( char* text )
{
	BOOST_LOG_SEV(slg, warning) << text;
}

void CLogManager::Error( char* text )
{
	BOOST_LOG_SEV(slg, error) << text;
}

void CLogManager::Fatal( char* text )
{
	BOOST_LOG_SEV(slg, fatal) << text;
}
