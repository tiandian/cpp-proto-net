#pragma once

enum
{
	ServerLoginRequestID = 1,
	ServerLogoutRequestID,
	AddPortfolioRequestID,
	RemovePortfolioRequestID,
	PortfolioSwitchRequestID,
	PortfolioSyncRequestID,
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