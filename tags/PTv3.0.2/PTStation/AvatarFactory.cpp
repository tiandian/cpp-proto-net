#include "StdAfx.h"
#include "AvatarFactory.h"
#include "AvatarClient.h"
#include "ProtobufPacket.h"
#include "ProtocolIDs.h"
#include "globalmembers.h"
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

	pPuzzle->getData().set_session_id(pCxt->puzzle);
	lpContext = pCxt;
	return pPuzzle;
}

int CAvatarFactory::onFirstRequest( IncomingPacket& request, ConnectionContext* lpContext, LogicalConnection*& lpClient, OutgoingPacket*& lpPacket )
{
	try
	{
		ProtobufPacket<entity::LoginRequest>& loginRequest= dynamic_cast<ProtobufPacket<entity::LoginRequest>&>(request);

		string question = ((CAvatarConnectionCxt*) lpContext)->puzzle;

		if (loginRequest.getData().session_id() != question )
		{
			ProtobufPacket<entity::LoginResponse>* pLoginResponse = 
				new ProtobufPacket<entity::LoginResponse>(LoginResponseID);

			pLoginResponse->getData().set_accepted(false);
			pLoginResponse->getData().set_session_id("");
			pLoginResponse->getData().set_is_new(loginRequest.getData().is_new());
			pLoginResponse->getData().set_error_msg("Failed to meet login challenge");

			lpPacket = pLoginResponse;

			return ClientFactory::RefuseRequest;
		}

		if(loginRequest.getData().is_new())
		{
			CAvatarClient* pClient = new CAvatarClient(loginRequest.getData().session_id());
			if(loginRequest.getData().has_pseudo())
			{
				pClient->Pseudo(loginRequest.getData().pseudo());
			}

			lpClient = pClient;
		}
		else
		{
			const string& previousSessionId = loginRequest.getData().previous_session_id();
			LogicalConnection* existingConn = FindClient(previousSessionId.c_str());
			CAvatarClient* pClient = dynamic_cast<CAvatarClient*>(existingConn);
			if(pClient != NULL)
			{
				lpClient = pClient;
			}
			else
			{
				ProtobufPacket<entity::LoginResponse>* pLoginResponse = 
					new ProtobufPacket<entity::LoginResponse>(LoginResponseID);

				pLoginResponse->getData().set_accepted(false);
				pLoginResponse->getData().set_session_id("");
				pLoginResponse->getData().set_is_new(loginRequest.getData().is_new());
				pLoginResponse->getData().set_error_msg("Cannot attached existing client");

				lpPacket = pLoginResponse;

				return ClientFactory::RefuseRequest;
			}

			if(existingConn != NULL)
				ReturnClient(existingConn);
		}

		return ClientFactory::CreateClient;
	}
	catch (std::bad_cast ex)	// in case the first request is not Login request
	{
		return ClientFactory::RefuseAndClose;
	}
}

void CAvatarFactory::onClientConnected( LogicalConnection* pClient )
{
	//When a client connects.
	CAvatarClient* pAvatar = (CAvatarClient*) pClient;

	ProtobufPacket<entity::LoginResponse> loginRespone(LoginResponseID);
	loginRespone.getData().set_accepted(true);
	loginRespone.getData().set_is_new(true);
	loginRespone.getData().set_session_id(pAvatar->getKey());
	loginRespone.getData().set_error_msg("");

	pAvatar->PushPacket(&loginRespone);
	LOG_INFO(logger, boost::str(boost::format("New client(%s) logged in with session id - %s") 
		% pAvatar->Pseudo() % pAvatar->getKey()));
}

void CAvatarFactory::onClientReconnected( LogicalConnection* pClient )
{
	//When a client get reconnect.
	CAvatarClient* pAvatar = (CAvatarClient*) pClient;

	ProtobufPacket<entity::LoginResponse> loginRespone(LoginResponseID);
	loginRespone.getData().set_accepted(true);
	loginRespone.getData().set_is_new(false);
	loginRespone.getData().set_session_id(pAvatar->getKey());
	loginRespone.getData().set_error_msg("");

	pAvatar->PushPacket(&loginRespone);
	LOG_INFO(logger, boost::str(boost::format("Existing client(%s) gets re-logged in with session id - %s") 
		% pAvatar->Pseudo() % pAvatar->getKey()));
}

const char* CLOSE_REASON_TEXT[] = {"PeerClosure", "InactiveClient", "ForceableClosure", "UnknownFailure"};

void CAvatarFactory::onClientDisconnected( LogicalConnection* pClient, CloseReason closeReason )
{
	CAvatarClient* pAvatar = (CAvatarClient*) pClient;
	LOG_INFO(logger, boost::str(boost::format("Client(%s) disconnected due to %s") 
		% pAvatar->Pseudo() % CLOSE_REASON_TEXT[closeReason]));
	
}

void CAvatarFactory::disposeClient( LogicalConnection* pClient )
{
	CAvatarClient* pAvatar = (CAvatarClient*) pClient;
	LOG_INFO(logger, boost::str(boost::format("Disposing client %s ...") % pAvatar->Pseudo()));
	delete pAvatar;
}
