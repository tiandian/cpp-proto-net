// This is the main DLL file.

#include "stdafx.h"

#include "ClientBase.h"

namespace PTCommunication {

bool ClientBase::Connect( String ^host, int port )
{
	IntPtr stringPointer;
	try
	{
		stringPointer = (IntPtr)Marshal::StringToHGlobalAnsi(host);
		bool succ = _navtiveClient->connect((char*)stringPointer.ToPointer(), port);
		return succ;
	}
	finally
	{
		Marshal::FreeHGlobal(stringPointer);
	}
}

void ClientBase::SetPseudo( String ^pseudo )
{
	IntPtr pseudoPointer;
	try
	{
		pseudoPointer = (IntPtr)Marshal::StringToHGlobalAnsi(pseudo);
		_navtiveClient->setPseudo((char*)pseudoPointer.ToPointer());
	}
	finally
	{
		Marshal::FreeHGlobal(pseudoPointer);
	}
}

}