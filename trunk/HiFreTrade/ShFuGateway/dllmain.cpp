// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "LogManager.h"

CLogManager	logger;


bool IsVistaOrLater()
{
	OSVERSIONINFO osinfo;

	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osinfo);

	return (osinfo.dwMajorVersion > 5);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if(IsVistaOrLater())
			logger.Init();
		break;
	case DLL_THREAD_ATTACH:
		break;		
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


