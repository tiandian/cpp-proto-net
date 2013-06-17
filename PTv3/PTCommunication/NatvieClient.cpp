#include "StdAfx.h"
#include "NatvieClient.h"
#include "ProtocolIDs.h"
#include "ResponseHandlers.h"
#include "ClientProtocol.h"
#include "ServerLoginReturn.h"
#include "ProtobufPacket.h"


CNatvieClient::CNatvieClient(PTCommunication::IClientRequestHandler ^reqHandler)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	setProtocol(new CClientProtocol);

	registerHandler(LoginResponseID, new LoginResponseHandler(this));
	registerHandler(LoginPuzzleResponseID, new LoginPuzzleResponseHandler(this));
	registerHandler(ServerLoginResponseID, new ServerLoginResponseHandler(this));

	m_clr = reqHandler;
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
	if(resp.accepted())
	{
		m_sessionId = resp.session_id();
	}

	msclr::auto_gcroot<LoginReturn^> loginRet = gcnew LoginReturn(&resp);
	m_clr->OnLoginReturned(loginRet.get());
}

void CNatvieClient::OnLoginPuzzleResponse( entity::LoginPuzzleResponse& resp )
{
	tryLogin(resp.session_id());
}

void CNatvieClient::OnServerLoginResponse( entity::ServerLoginResponse& resp )
{
	 msclr::auto_gcroot<ServerLoginReturn^> loginRet = gcnew ServerLoginReturn();
	 loginRet->From(&resp);
	 m_clr->OnServerLoginReturned(loginRet.get());
}

bool CNatvieClient::tryLogin(const string& sessionId)
{
	if (getStatus() == Disconnected)
	{
		return false;
	}

	if (getStatus() != Connected)
	{
		return false;
	}

	ProtobufPacket<entity::LoginRequest> request(LoginRequestID);
	
	request.getData().set_is_new(true);
	request.getData().set_session_id(sessionId);
	request.getData().set_previous_session_id("");
	request.getData().set_pseudo(m_pseudo);

	return sendRequest(&request) > 0;
}

bool CNatvieClient::logged()
{
	return getStatus() == Connected && m_sessionId.length() > 0;
}
