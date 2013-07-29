#pragma once

enum
{
	LoginRequestID = 1,
	ServerLoginRequestID,
	ServerLogoutRequestID,
	AddPortfolioRequestID,
	RemovePortfolioRequestID,
	PortfolioSwitchRequestID,
	PortfolioSyncRequestID,
	ApplyStrategySetttingRequestID,
	HeartbeatRequestID,
};

enum
{
	LoginPuzzleResponseID = 1,
	LoginResponseID,
	ServerLoginResponseID,
	ServerLogoutResponseID,
	PortfolioUpdateResponseID,
	MultilegOrderResponseID,
	LegOrderResponseID,
	TradeResponseID,
	HeartbeatResponseID,
};