#include "StdAfx.h"
#include "TradingServer.h"
#include "ProtocolIDs.h"
#include "RequestedServices.h"

CTradingServer::CTradingServer(void)
{
	setServerInfos("Portfolio Trading Server");
}


CTradingServer::~CTradingServer(void)
{
}

void CTradingServer::CreateServices()
{
	registerService(ServerLoginRequestID, new ServerLoginService(), "serverLogin", true);
	registerService(ServerLogoutRequestID, new ServerLogoutService(), "serverLogout", true);
	registerService(AddPortfolioRequestID, new AddPortfolioService(), "addPortfolio", true);
	registerService(PortfolioSwitchRequestID, new PortfolioSwitchService(), "switchPortfolio", true);
	registerService(ApplyStrategySetttingRequestID, new ApplyStrategySettingsService(), "applyStrategySettings", true);
	registerService(HeartbeatRequestID, new HeartbeatService(), "heartbeat", true);
	registerService(PortfolioModifyQtyRequestID, new PortfModifyQtyService(), "modifyPortfolioQuantity", true);
}
