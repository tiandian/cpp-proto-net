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
