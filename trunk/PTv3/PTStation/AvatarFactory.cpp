#include "StdAfx.h"
#include "AvatarFactory.h"
#include "AvatarClient.h"
#include "ProtobufPacket.h"
#include "ProtocolIDs.h"
#include "entity/message.pb.h"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

CAvatarFactory::CAvatarFactory(void)
{
}


CAvatarFactory::~CAvatarFactory(void)
{
}

OutgoingPacket* CAvatarFactory::onNewConnection( ConnectionContext*& lpContext )
{
	//Send the login challenge :
	ProtobufPacket<entity::LoginPuzzleResponse>* pPuzzle = 
		new ProtobufPacket<entity::LoginPuzzleResponse>(LoginPuzzleResponseID);

	CAvatarConnectionCxt* pCxt = new CAvatarConnectionCxt;
	boost::uuids::random_generator gen;
	boost::uuids::uuid uuid_ = gen();
	string sessionId = boost::uuids::to_string(uuid_);
	pCxt->puzzle = sessionId;

	pPuzzle->getData().set_clientkey(pCxt->puzzle);

	lpContext = pCxt;
	return pPuzzle;
}

void CAvatarFactory::onClientConnected( LogicalConnection* pClient )
{
	//When a client connects.
	CAvatarClient* pAvatar = (CAvatarClient*) pClient;

	ProtobufPacket<entity::LoginResponse> loginRespone(LoginResponseID);
	loginRespone.getData().set_accepted(true);
	loginRespone.getData().set_pseudo("welcome to server");

	pAvatar->PushPacket(&loginRespone);
}

int CAvatarFactory::onFirstRequest( IncomingPacket& request, ConnectionContext* lpContext, LogicalConnection*& lpClient, OutgoingPacket*& lpPacket )
{
	ProtobufPacket<entity::LoginRequest>& loginRequest= (ProtobufPacket<entity::LoginRequest>&) request;

	string question = ((CAvatarConnectionCxt*) lpContext)->puzzle;

	if (loginRequest.getData().clientkey() != question )
	{
		ProtobufPacket<entity::LoginResponse>* pLoginResponse = 
			new ProtobufPacket<entity::LoginResponse>(LoginResponseID);

		pLoginResponse->getData().set_accepted(false);
		pLoginResponse->getData().set_pseudo("login puzzle is false");

		lpPacket = pLoginResponse;

		return ClientFactory::RefuseRequest;
	}


	CAvatarClient* pClient = new CAvatarClient(loginRequest.getData().clientkey());
	lpClient = pClient;

	return ClientFactory::CreateClient;
}


void CAvatarFactory::onClientDisconnected( LogicalConnection* pClient, CloseReason closeReason )
{

}
