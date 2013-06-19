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

void ClientBase::ServerLogin( ServerType svrType, String ^address, String ^brokerId, String ^investorId, String ^password )
{
	IntPtr pAddress, pBrokerId, pInvestorId, pPassword;
	try
	{
		pAddress = (IntPtr)Marshal::StringToHGlobalAnsi(address);
		pBrokerId = (IntPtr)Marshal::StringToHGlobalAnsi(brokerId);
		pInvestorId = (IntPtr)Marshal::StringToHGlobalAnsi(investorId);
		pPassword = (IntPtr)Marshal::StringToHGlobalAnsi(password);

		entity::ServerType eSvrType = static_cast<entity::ServerType>(svrType);

		_navtiveClient->ServerLogin(eSvrType,
									(char*)pAddress.ToPointer(),
									(char*)pBrokerId.ToPointer(),
									(char*)pInvestorId.ToPointer(),
									(char*)pPassword.ToPointer());
	}
	finally
	{
		Marshal::FreeHGlobal(pAddress);
		Marshal::FreeHGlobal(pBrokerId);
		Marshal::FreeHGlobal(pInvestorId);
		Marshal::FreeHGlobal(pPassword);
	}
}

void ClientBase::ServerLogout( ServerType svrType )
{
	entity::ServerType eSvrType = static_cast<entity::ServerType>(svrType);
	_navtiveClient->ServerLogout(eSvrType);
}

void ClientBase::Disconnect()
{
	if(this->IsConnected)
		_navtiveClient->Logout();
}

}