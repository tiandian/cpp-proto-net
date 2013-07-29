#include "StdAfx.h"
#include "QS_Configuration.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

CQSConfiguration::CQSConfiguration(void)
	: m_testHost(false)
{
}

CQSConfiguration::~CQSConfiguration(void)
{
}

bool CQSConfiguration::Load( int argc, char* argv[] )
{
	try {
		// Declare a group of options that will be 
		// allowed only on command line
		po::options_description generic("Startup options");
		generic.add_options()
			("conn,c", po::value<string>(&m_connectionString),
				"Connection string of quote server")
			("bid,b", po::value<string>(&m_brokerId),
				"Broker Id for quote login")
			("user,u", po::value<string>(&m_username),
				"Account for quote login")
			("pwd,p", po::value<string>(&m_password),
				"Password for quote login")
			("test,t", po::value<bool>(&m_testHost)->default_value(false),
				"Indicates whether it's a test host");

		po::options_description cmdline_options;
		cmdline_options.add(generic);

		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
		po::notify(vm);

		if(m_connectionString.length() > 0 
			&& m_brokerId.length() > 0 
			&& m_username.length() > 0
			&& m_password.length() > 0)
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
