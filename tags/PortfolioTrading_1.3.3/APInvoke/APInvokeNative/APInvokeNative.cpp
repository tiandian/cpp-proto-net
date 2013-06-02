// APInvokeNative.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "APInvokeNative.h"


// This is an example of an exported variable
APINVOKENATIVE_API int nAPInvokeNative=0;

// This is an example of an exported function.
APINVOKENATIVE_API int fnAPInvokeNative(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see APInvokeNative.h for the class definition
CAPInvokeNative::CAPInvokeNative()
{
	return;
}
