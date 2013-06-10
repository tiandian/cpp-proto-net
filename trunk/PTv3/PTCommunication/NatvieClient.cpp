#include "StdAfx.h"
#include "NatvieClient.h"
#include "ProtocolIDs.h"
#include "ResponseHandlers.h"
#include "ClientProtocol.h"


CNatvieClient::CNatvieClient(void)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	setProtocol(new CClientProtocol);

	registerHandler(LoginResponseID, new LoginResponseHandler(this));
	registerHandler(LoginPuzzleResponseID, new LoginPuzzleResponseHandler(this));
	registerHandler(ServerLoginResponseID, new ServerLoginResponseHandler(this));
}


CNatvieClient::~CNatvieClient(void)
{
}

void CNatvieClient::onConnected()
{

}

void CNatvieClient::onConnectionClosed()
{

}

void CNatvieClient::OnLoginResponse( entity::LoginResponse& resp )
{

}

void CNatvieClient::OnLoginPuzzleResponse( entity::LoginPuzzleResponse& resp )
{

}

void CNatvieClient::OnServerLoginResponse( entity::ServerLoginResponse& resp )
{

}
