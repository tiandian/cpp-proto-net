// PTStation.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"

#include "LogManager.h"
#include "Configuration.h"
#include "TradingServer.h"
#include "AvatarFactory.h"
#include "TradingServerProtocol.h"

CConfiguration config;
CLogManager	logger;

int main(int argc, char* argv[])
{
	cout << "Trade Station v" << APP_VERSION << endl;

	if(!config.Load(argc, argv))
		return 1;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	logger.Init();

	CTradingServer server;
	server.setClientFactory(new CAvatarFactory(), true);
	server.CreateServices();

	ListenerOptions lOptions;
	lOptions.pProtocol = new CTradingServerProtocol();
	server.createListener(config.GetPortNum(), &lOptions);

	try
	{
		logger.Info(boost::str(boost::format("Server starting listen at %s") % config.GetPort()));
		server.start(false);
	}
	catch (std::exception& e)
	{
		cout << "Failed to start server. Exception : " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

