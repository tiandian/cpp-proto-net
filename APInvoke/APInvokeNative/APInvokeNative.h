#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the APINVOKENATIVE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// APINVOKENATIVE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef APINVOKENATIVE_EXPORTS
#define APINVOKENATIVE_API __declspec(dllexport)
#else
#define APINVOKENATIVE_API __declspec(dllimport)
#endif

#include <string>

using namespace std;

// This class is exported from the APInvokeNative.dll
class APINVOKENATIVE_API CAPInvokeNative {
public:
	CAPInvokeNative(void);
	// TODO: add your methods here.
};

class APINVOKENATIVE_API Session
{
public:
	virtual ~Session(void){}

	virtual const string& SessionId() = 0; 
};

class SessionManagerHandler
{
public:
	virtual void OnConnected(const Session* session){}

	virtual void OnDisconnected(const Session* session){}

	virtual void OnError(const Session* session, const string& errorMsg){}
};

class APINVOKENATIVE_API SessionManager
{
public:
	static SessionManager *Create();
	
	virtual ~SessionManager(void){}

	virtual void RegisterCallback(SessionManagerHandler* handler) = 0;

	virtual bool Listen(unsigned int nPort) = 0;

	virtual void Close() = 0;

};

extern APINVOKENATIVE_API int nAPInvokeNative;

APINVOKENATIVE_API int fnAPInvokeNative(void);
