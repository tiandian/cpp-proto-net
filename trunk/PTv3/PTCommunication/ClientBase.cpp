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
		bool succ = _nativeClient->connect((char*)stringPointer.ToPointer(), port);
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
		_nativeClient->setPseudo((char*)pseudoPointer.ToPointer());
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

		_nativeClient->ServerLogin(eSvrType,
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
	_nativeClient->ServerLogout(eSvrType);
}

void ClientBase::Disconnect()
{
	if(this->IsConnected)
		_nativeClient->Logout();
}

void ClientBase::AddPortfolio( PortfolioItem ^portfItem )
{
	_nativeClient->AddPortfolio(portfItem);
}

void ClientBase::AddPortfCollection( array<PortfolioItem ^> ^portfItems )
{
	_nativeClient->AddPortfolios(portfItems);
}

void ClientBase::PortfEnableStrategy( String ^portfId, bool isEnabled )
{
	IntPtr pPortfIdAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(portfId);
		_nativeClient->PortfEnableStrategy((char*)pPortfIdAddress.ToPointer(), isEnabled);
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
	}
}

void ClientBase::PortfTurnSwitches( String ^portfId, int triggerIndex, bool enabled )
{
	IntPtr pPortfIdAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(portfId);
		_nativeClient->PortfTurnSwitches((char*)pPortfIdAddress.ToPointer(), triggerIndex, enabled);
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
	}
}

void ClientBase::PortfApplyStrategySettings( String ^portfId, StrategyItem ^strategyItem )
{
	IntPtr pPortfIdAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(portfId);
		_nativeClient->ApplyStrategySettings((char*)pPortfIdAddress.ToPointer(), strategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
	}
}

}