#pragma once

#include "IClientRequestHandler.h"
#include "tcpsocket/TCPSocket.h"
#include "entity/message.pb.h"

#include <gcroot.h>
#include <msclr/auto_gcroot.h>

class CNatvieClient : public TCPSocket
{
public:
	CNatvieClient(PTCommunication::IClientRequestHandler ^reqHandler);
	~CNatvieClient(void);

	void OnLoginResponse(entity::LoginResponse& resp);
	void OnLoginPuzzleResponse(entity::LoginPuzzleResponse& resp);
	void OnServerLoginResponse(entity::ServerLoginResponse& resp);

	bool logged();

	void setPseudo(const char* pseudo){ m_pseudo = pseudo; }

protected:
	virtual void onConnected();
	virtual void onConnectionClosed();

private:
	bool tryLogin(const string& clientKey);


	msclr::auto_gcroot<PTCommunication::IClientRequestHandler^> m_clr;

	string m_sessionId;
	string m_pseudo;
};

