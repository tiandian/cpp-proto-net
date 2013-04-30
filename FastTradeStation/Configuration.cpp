#include "Configuration.h"

#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <boost/date_time.hpp>
#include <boost/program_options.hpp>

#define APP_VERSION "0.1.704"
#define DEFAULT_LOG_PATH "~/fast-trade/log4cpp.property"

namespace po = boost::program_options;
using namespace std;

CConfiguration::CConfiguration(void):
	m_enableLogging(true),
	m_logConfigFilePath(DEFAULT_LOG_PATH)
{
}

CConfiguration::~CConfiguration(void)
{
}

bool CConfiguration::Load( int argc, char* argv[] )
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
			"Logging enable or not")
			("logConfig", po::value<string>(&m_logConfigFilePath)->default_value(DEFAULT_LOG_PATH), 
			"Specify log config file path")
			;

		po::options_description conn("Connection");
		conn.add_options()
			("port", po::value<string>(&m_port)->default_value("16181"),
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

const string& CConfiguration::LogConfigPath()
{
	return m_logConfigFilePath;
}

const string& CConfiguration::GetPort()
{
	return m_port;
}

const string& CConfiguration::GetAddr()
{
	return m_addr;
}

