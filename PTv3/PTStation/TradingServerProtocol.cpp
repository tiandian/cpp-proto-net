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
	case AddPortfolioRequestID:
		return new ProtobufPacket<entity::AddPortfolioRequest>(serviceId);
	case PortfolioSwitchRequestID:
		return new ProtobufPacket<entity::SwitchPortfolioRequest>(serviceId);
	case ApplyStrategySetttingRequestID:
		return new ProtobufPacket<entity::ApplyStrategySettingsRequest>(serviceId);
	case HeartbeatRequestID:
		return new ProtobufPacket<entity::HeartbeatRequest>(serviceId);
	case PortfolioModifyQtyRequestID:
		return new ProtobufPacket<entity::ModifyPortfolioQtyParam>(serviceId);
	case PortfolioModifyPreferredLegRequestID:
		return new ProtobufPacket<entity::ModifyPortfolioPreferredLegParam>(serviceId);
	case PortfolioOpenPositionRequestID:
		return new ProtobufPacket<entity::PortfOpenPosiParam>(serviceId);
	case PortfolioClosePositionRequestID:
		return new ProtobufPacket<entity::ClosePositionParam>(serviceId);
	default:
		assert(false);
		return NULL;
	}
}
