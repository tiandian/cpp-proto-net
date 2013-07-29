#include "StdAfx.h"
#include "NatvieClient.h"
#include "ResponseHandlers.h"
#include "ProtobufPacket.h"
#include "entity/message.pb.h"


void LoginResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<entity::LoginResponse>& response = (ProtobufPacket<entity::LoginResponse>&) packet;
	m_pSession->OnLoginResponse(response.getData());
}

void LoginPuzzleResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<entity::LoginPuzzleResponse>& response = (ProtobufPacket<entity::LoginPuzzleResponse>&) packet;
	m_pSession->OnLoginPuzzleResponse(response.getData());
}

void ServerLoginResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<entity::ServerLoginResponse>& response = (ProtobufPacket<entity::ServerLoginResponse>&) packet;
	m_pSession->OnServerLoginResponse(response.getData());
}

void PortfolioUpdateResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<entity::PortfolioUpdateItem>& resp = (ProtobufPacket<entity::PortfolioUpdateItem>&) packet;
	m_pSession->OnPortfolioUpdateResponse(resp.getData());
}

void MultilegOrderUpdateResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<trade::MultiLegOrder>& resp = (ProtobufPacket<trade::MultiLegOrder>&) packet;
	m_pSession->OnMultilegOrderUpdateResponse(resp.getData());
}

void LegOrderUpdateResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<entity::LegOrderUpdateParam>& resp = (ProtobufPacket<entity::LegOrderUpdateParam>&) packet;
	m_pSession->OnLegOrderUpdateResponse(resp.getData());
}

void TradeUpdateResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<trade::Trade>& resp = (ProtobufPacket<trade::Trade>&) packet;
	m_pSession->OnTradeUpdateResponse(resp.getData());
}

void HeartbeatResponseHandler::handleResponse( PushFramework::IncomingPacket& packet )
{
	ProtobufPacket<entity::HeartbeatResponse>& resp = (ProtobufPacket<entity::HeartbeatResponse>&)packet;
	m_pSession->OnHeartbeatResponse(resp.getData().timestamp().c_str());
}
