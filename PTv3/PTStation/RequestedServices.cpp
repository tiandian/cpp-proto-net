#include "StdAfx.h"
#include "RequestedServices.h"
#include "AvatarClient.h"
#include "ProtobufPacket.h"
#include "ProtocolIDs.h"
#include "globalmembers.h"
#include "entity/message.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>

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
		avatarClient->PortfolioManager().AddPortfolio(avatarClient, addPortfReq.portfolios(i));
	}
}

void RemovePortfolioService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	
}

void PortfolioSwitchService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	CAvatarClient* avatarClient = (CAvatarClient*)pClient;
	ProtobufPacket<entity::SwitchPortfolioRequest>* pReqPacket = (ProtobufPacket<entity::SwitchPortfolioRequest>*)pRequest;
	entity::SwitchPortfolioRequest& switchPortfReq = pReqPacket->getData();
	CPortfolio* pPortf = avatarClient->PortfolioManager().Get(switchPortfReq.pid());
	if(pPortf != NULL)
	{
		if(switchPortfReq.switchtype() == entity::STRATEGY_SWITCH && switchPortfReq.has_startstrategy())
		{
			if(switchPortfReq.startstrategy())
			{
				int lastOrderId = switchPortfReq.has_lastorderid() ? switchPortfReq.lastorderid() : 0;
				pPortf->StartStrategy(lastOrderId);
			}
			else
			{
				pPortf->StopStrategy();
			}
		}
		else if(switchPortfReq.switchtype() == entity::TRIGGER_SWITCH 
			&& switchPortfReq.has_triggerindex() && switchPortfReq.has_enabletrigger())
		{
			pPortf->EnableTrigger(switchPortfReq.triggerindex(), switchPortfReq.enabletrigger());
		}
	}
}

void PortfolioSyncService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{

}

void ApplyStrategySettingsService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	CAvatarClient* avatarClient = (CAvatarClient*)pClient;
	ProtobufPacket<entity::ApplyStrategySettingsRequest>* pReqPacket = (ProtobufPacket<entity::ApplyStrategySettingsRequest>*)pRequest;
	entity::ApplyStrategySettingsRequest& applyStrategyReq = pReqPacket->getData();
	CPortfolio* pPortf = avatarClient->PortfolioManager().Get(applyStrategyReq.pid());
	logger.Debug(boost::str(boost::format("[%s] Applying Strategy setting change for portfolio %s")
		% avatarClient->Pseudo() % pPortf->ID()));
	pPortf->Strategy()->Apply(applyStrategyReq.strategy(), true);
}

void HeartbeatService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	ProtobufPacket<entity::HeartbeatRequest>* heartbeatReq = (ProtobufPacket<entity::HeartbeatRequest>*)pRequest;
	
	ProtobufPacket<entity::HeartbeatResponse> resp(HeartbeatResponseID);
	string tsSvr = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
	resp.getData().set_timestamp(tsSvr);
	if(!pClient->IsInactive())
		pClient->PushPacket(&resp);
}

void PortfModifyQtyService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	ProtobufPacket<entity::ModifyPortfolioQtyParam>* pReqPacket = (ProtobufPacket<entity::ModifyPortfolioQtyParam>*)pRequest;
	entity::ModifyPortfolioQtyParam& modifyQtyParam = pReqPacket->getData();
	
	CAvatarClient* avatarClient = (CAvatarClient*)pClient;
	CPortfolio* pPortf = avatarClient->PortfolioManager().Get(modifyQtyParam.portfid());
	pPortf->SetQuantity(modifyQtyParam.peropenqty(), modifyQtyParam.perstartqty(), 
		modifyQtyParam.totalopenlimit(), modifyQtyParam.maxcancelqty());

	vector<string> timepointVec;
	int count = modifyQtyParam.endtimepoints_size();
	if(count > 0)
	{
		for(int i = 0; i < count; ++i)
		{
			timepointVec.push_back(modifyQtyParam.endtimepoints(i));
		}
	}
	pPortf->SetEndTimePoints(timepointVec);
}
