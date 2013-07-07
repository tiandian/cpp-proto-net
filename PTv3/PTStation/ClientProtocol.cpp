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

	default:
		return NULL;
	}
}
