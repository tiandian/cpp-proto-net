#include "StdAfx.h"
#include "Configuration.h"

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#define APP_VERSION "0.2.308"

namespace po = boost::program_options;
using namespace std;

CConfiguration::CConfiguration(void):
	m_enableLogging(true),
	m_logFilePath("log.txt"),
	m_logLevel(trace)
{
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
			("help,h", "produce help message")
			("ctrl", po::value<string>(&m_controlType)->default_value(CONSOLE), "how to manipulate this program, command or remote")
			("config,c", po::value<string>(&configFile)->default_value("TradeBuffer.cfg"),
			"path of a file of a configuration.")
			;

		// Declare a group of options that will be 
		// allowed both on command line and in
		// config file
		po::options_description logging("Logging");
		logging.add_options()
			("enableLogging", po::value<bool>(&m_enableLogging)->default_value(true), 
			"logging enable or not")
			("logFilePath", po::value<string>(&m_logFilePath)->default_value("log.txt"), 
			"specify log file path")
			("logLevel", po::value<severity_level>(&m_logLevel)->default_value(trace), 
			"specify minimum severity to log")
			;

		po::options_description quote("Quote");
		quote.add_options()
			("marketDataAddress", 
			po::value<string>(&m_marketDataAddress)->default_value("tcp://asp-sim2-md1.financial-trading-platform.com:26213"),
			"remote market data address to connect")
			("brokerID", po::value<string>(&m_brokerID)->default_value("2030"),
			"broker's unique ID")
			("investorID", po::value<string>(&m_investorID)->default_value("00092"),
			"investor's account ID")
			("password", po::value<string>(&m_password)->default_value("888888"),
			"password corresponding to account")
			;

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(logging).add(quote);

		po::options_description config_file_options;
		config_file_options.add(logging).add(quote);

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

		std::ifstream ifs(configFile.c_str());
		if (!ifs)
		{
			cout << "can not open config file: " << configFile.c_str() << endl;
			cout << "application is running with default arguments, only able to connect simulating market." << endl;
		}
		else
		{
			store(parse_config_file(ifs, config_file_options), vm);
			notify(vm);
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

const char* CConfiguration::GetMarketDataAddr()
{
	return m_marketDataAddress.c_str();
}

const char* CConfiguration::GetBrokerID()
{
	return m_brokerID.c_str();
}

const char* CConfiguration::GetInvestorID()
{
	return m_investorID.c_str();
}

const char* CConfiguration::GetPassword()
{
	return m_password.c_str();
}

const char* CConfiguration::GetControlType()
{
	return m_controlType.c_str();
}


