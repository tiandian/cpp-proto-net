#include "StdAfx.h"
#include "Configuration.h"

#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <boost/date_time.hpp>
#include <boost/program_options.hpp>

#define APP_VERSION "0.1.704"
#define DEFAULT_LOG_PATH "log\\ts_%N.txt"

namespace po = boost::program_options;
using namespace std;

std::string DATETIME_LOG_PATH;

void GetCurrentDateTimeString()
{
	//boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	//string dateText = boost::gregorian::to_iso_string(d);
	boost::posix_time::ptime t1 = boost::posix_time::second_clock::local_time();
	string timeText = boost::posix_time::to_iso_string(t1);
	DATETIME_LOG_PATH = boost::str(boost::format("log\\ts_%s_%%N.txt")
		% timeText);
}

CConfiguration::CConfiguration(void):
	m_enableLogging(true),
	//m_logFilePath(DEFAULT_LOG_PATH),
	m_logLevel(trace)
{
	GetCurrentDateTimeString();
	m_logFilePath = DATETIME_LOG_PATH;
}

CConfiguration::~CConfiguration(void)
{
}

bool CConfiguration::Load( int argc, _TCHAR* argv[] )
{
	try {
		string configFile;
		// Declare a group of options that will be 
		// allowed only on command line
		po::options_description generic("Startup options");
		generic.add_options()
			("version,v", "print version string")
			("help,h", "produce help message");

		// Declare a group of options that will be 
		// allowed both on command line and in
		// config file
		po::options_description logging("Logging");
		logging.add_options()
			("enableLogging", po::value<bool>(&m_enableLogging)->default_value(true), 
			"logging enable or not")
			("logFilePath", po::value<string>(&m_logFilePath)->default_value(DATETIME_LOG_PATH), 
			"specify log file path")
			("logLevel", po::value<severity_level>(&m_logLevel)->default_value(trace), 
			"specify minimum severity to log")
			;

		po::options_description conn("Connection");
		conn.add_options()
			("port", po::value<string>(&m_port)->default_value("16168"),
			"listening port")
			("addr", po::value<string>(&m_addr)->default_value(""),
			"listening port");

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(logging).add(conn);

		//po::options_description config_file_options;
		//config_file_options.add(logging).add(conn);

		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
		po::notify(vm);

		if (vm.count("version")){
			cout << APP_VERSION << endl;
			return false;
		}

		if (vm.count("help")) {
			cout << cmdline_options << endl;
			return false;
		}

		return true;
	}
	catch(exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	catch(...) {
		cerr << "Exception of unknown type!" << endl;
	}
	// must be something wrong, thus get here
	return false;
}

bool CConfiguration::IsLogEnabled()
{
	return m_enableLogging;
}

const char* CConfiguration::GetLogFilePath()
{
	return m_logFilePath.c_str();
}

severity_level CConfiguration::GetLogLevel()
{
	return m_logLevel;
}

const string& CConfiguration::GetPort()
{
	return m_port;
}

const string& CConfiguration::GetAddr()
{
	return m_addr;
}

