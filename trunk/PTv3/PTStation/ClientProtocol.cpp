#include "StdAfx.h"
#include "ClientProtocol.h"
#include "ProtobufProtocol.h"
#include "ProtocolIDs.h"
#include "ProtobufPacket.h"
#include "entity/message.pb.h"

CClientProtocol::CClientProtocol(void)
{
}


CClientProtocol::~CClientProtocol(void)
{
}

ProtobufPacketImpl* CClientProtocol::createIncomingPacketFromServiceId( int serviceId )
{
	switch (serviceId)
	{
	case LoginPuzzleResponseID:
		return new ProtobufPacket<entity::LoginPuzzleResponse>(serviceId);
	case LoginResponseID:
		return new ProtobufPacket<entity::LoginResponse>(serviceId);
	case ServerLoginResponseID:
		return new ProtobufPacket<entity::ServerLoginResponse>(serviceId);
	case PortfolioUpdateResponseID:
		return new ProtobufPacket<entity::PortfolioUpdateItem>(serviceId);
	case MultilegOrderResponseID:
		return new ProtobufPacket<trade::MultiLegOrder>(serviceId);
	case LegOrderResponseID:
		return new ProtobufPacket<entity::LegOrderUpdateParam>(serviceId);
	case TradeResponseID:
		return new ProtobufPacket<trade::Trade>(serviceId);
	case HeartbeatResponseID:
		return new ProtobufPacket<entity::HeartbeatResponse>(serviceId);
	default:
		return NULL;
	}
}
