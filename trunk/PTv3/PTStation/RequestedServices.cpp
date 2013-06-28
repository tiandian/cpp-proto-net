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

	entity::ServerType svrType = pSvrLoginRequest->getData().type();
	if(svrType == entity::SERV_TRADE)
	{
		boost::tuple<bool, string> result = avatarClient->TradeLogin(pSvrLoginRequest->getData().address(), 
			pSvrLoginRequest->getData().brokerid(), pSvrLoginRequest->getData().userid(), pSvrLoginRequest->getData().password());
		response.getData().set_success(boost::get<0>(result));
		response.getData().set_errormessage(boost::get<1>(result));
	}
	else if(svrType == entity::SERV_QUOTE)
	{
		boost::tuple<bool, string> result = avatarClient->QuoteLogin(pSvrLoginRequest->getData().address(), 
			pSvrLoginRequest->getData().brokerid(), pSvrLoginRequest->getData().userid(), pSvrLoginRequest->getData().password());
		response.getData().set_success(boost::get<0>(result));
		response.getData().set_errormessage(boost::get<1>(result));
	}
	else
	{
		response.getData().set_success(false);
		response.getData().set_errormessage("Unexpected server type");
	}
	
	response.getData().set_type(pSvrLoginRequest->getData().type());
	response.getData().set_address(pSvrLoginRequest->getData().address());
	response.getData().set_brokerid(pSvrLoginRequest->getData().brokerid());
	response.getData().set_userid(pSvrLoginRequest->getData().userid());
	
	pClient->PushPacket(&response);
}

void ServerLogoutService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	CAvatarClient* avatarClient = (CAvatarClient*)pClient;

	ProtobufPacket<entity::ServerLogoutRequest>* pSvrLogoutRequest = (ProtobufPacket<entity::ServerLogoutRequest>*) pRequest;
	entity::ServerType svrType = pSvrLogoutRequest->getData().type();
	switch(svrType)
	{
	case entity::SERV_QUOTE:
		avatarClient->QuoteLogout();
		break;
	case entity::SERV_TRADE:
		avatarClient->TradeLogout();
		break;
	}
}

void AddPortfolioService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	CAvatarClient* avatarClient = (CAvatarClient*)pClient;
	ProtobufPacket<entity::AddPortfolioRequest>* pAddPortfReqPacket = (ProtobufPacket<entity::AddPortfolioRequest>*)pRequest;
	entity::AddPortfolioRequest& addPortfReq = pAddPortfReqPacket->getData();
	int portfCount = addPortfReq.portfolios_size();

	for(int i = 0; i < portfCount; ++i)
	{
		avatarClient->PortfolioManager().AddPortfolio(addPortfReq.portfolios(i));
	}
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
