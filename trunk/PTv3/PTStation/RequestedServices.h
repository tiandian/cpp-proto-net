#pragma once

class ServerLoginService : public Service
{
public:
	ServerLoginService(void){};
	~ServerLoginService(void){};

protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class ServerLogoutService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class AddPortfolioService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class RemovePortfolioService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class PortfolioSwitchService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class PortfolioSyncService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class ApplyStrategySettingsService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class HeartbeatService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class PortfModifyQtyService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class PortfModifyPreferredLegService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class PortfOpenPosiService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};

class PortfClosePosiService : public Service
{
protected:
	virtual void handle(LogicalConnection* pClient, IncomingPacket* pRequest);
};