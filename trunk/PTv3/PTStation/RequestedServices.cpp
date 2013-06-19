#include "StdAfx.h"
#include "RequestedServices.h"
#include "AvatarClient.h"
#include "ProtobufPacket.h"
#include "ProtocolIDs.h"
#include "entity/message.pb.h"

void ServerLoginService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	CAvatarClient* avatarClient = (CAvatarClient*)pClient;
	
	ProtobufPacket<entity::ServerLoginRequest>* pSvrLoginRequest = (ProtobufPacket<entity::ServerLoginRequest>*) pRequest;

	ProtobufPacket<entity::ServerLoginResponse> response(ServerLoginResponseID);
	response.getData().set_success(true);
	response.getData().set_errormessage("");
	response.getData().set_type(pSvrLoginRequest->getData().type());
	response.getData().set_address(pSvrLoginRequest->getData().address());
	response.getData().set_brokerid(pSvrLoginRequest->getData().brokerid());
	response.getData().set_userid(pSvrLoginRequest->getData().userid());
	
	pClient->PushPacket(&response);
}

void ServerLogoutService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{

}

void AddPortfolioService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{

}

void RemovePortfolioService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{

}

void PortfolioSwitchService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{

}

void PortfolioSyncService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{

}
