#pragma once

#include "tcpsocket/TCPSocket.h"
#include "entity/message.pb.h"

class CNatvieClient : public TCPSocket
{
public:
	CNatvieClient(void);
	~CNatvieClient(void);

	void OnLoginResponse(entity::LoginResponse& resp);
	void OnLoginPuzzleResponse(entity::LoginPuzzleResponse& resp);
	void OnServerLoginResponse(entity::ServerLoginResponse& resp);

protected:
	virtual void onConnected();
	virtual void onConnectionClosed();

	
};

