#include "StdAfx.h"
#include "TradingServerProtocol.h"
#include "ProtocolIDs.h"
#include "ProtobufPacket.h"
#include "entity/message.pb.h"

CTradingServerProtocol::CTradingServerProtocol(void)
{
}

CTradingServerProtocol::~CTradingServerProtocol(void)
{
}

ProtobufPacketImpl* CTradingServerProtocol::createIncomingPacketFromServiceId( int serviceId )
{
	switch (serviceId)
	{
	case LoginRequestID:
		return new ProtobufPacket<entity::LoginRequest>(serviceId);
	case ServerLoginRequestID:
		return new ProtobufPacket<entity::ServerLoginRequest>(serviceId);
	case ServerLogoutRequestID:
		return new ProtobufPacket<entity::ServerLogoutRequest>(serviceId);
	default:
		return NULL;
	}
}
