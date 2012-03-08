// TradeBuffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Configuration.h"
#include "LogManager.h"
#include "ConnectionManager.h"
#include "MarketAgent.h"
#include <google/protobuf/stubs/common.h>

#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")

CConfiguration config;
CLogManager	logger;
CConnectionManager g_connMgr;
CMarketAgent g_marketAgent;

int _tmain(int argc, _TCHAR* argv[])
{
	if(!config.Load(argc, argv))
		return 1;

	logger.Init();

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	logger.Trace("trace message");
	logger.Debug("Debug message");
	logger.Info("Infor message");
	logger.Warning("Warning message");
	logger.Error("Error message");
	logger.Fatal("Fatal message");



	return 0;
}

