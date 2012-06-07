// NativeServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServerSessionCallback.h"
#include "../APInvokeNative/APInvokeNative.h"

#pragma comment(lib, "../APInvokeNative/lib/APInvokeNative.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	ServerSessionCallback sessionCallback;
	SessionManager* pSessionManager = SessionManager::Create();
	pSessionManager->RegisterHandler(&sessionCallback);
	pSessionManager->Listen(16888);
	delete pSessionManager;
	return 0;
}

