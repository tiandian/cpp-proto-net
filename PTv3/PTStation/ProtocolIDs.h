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
};

enum
{
	LoginPuzzleResponseID = 1,
	LoginResponseID,
	ServerLoginResponseID,
	ServerLogoutResponseID,
	PortfolioUpdateResponseID,
	LegUpdateResponseID,
	MultilegOrderResponseID,
	LegOrderResponseID,
	TradeResponseID,
};