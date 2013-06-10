#include "StdAfx.h"
#include "NatvieClient.h"

CNatvieClient::CNatvieClient(void)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
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
