// This is the main DLL file.

#include "stdafx.h"

#include "ClientBase.h"
#include <string>
#include <vector>

namespace PTCommunication {

bool ClientBase::Connect( String ^host, int port )
{
	IntPtr stringPointer;
	try
	{
		if(!_nativeClient->IsDisconnected())
		{
			_nativeClient->disconnect(false);
		}
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
	if(this->IsConnected)
	{
		entity::ServerType eSvrType = static_cast<entity::ServerType>(svrType);
		_nativeClient->ServerLogout(eSvrType);
	}
}

void ClientBase::Disconnect()
{
	if(this->IsConnected)
		_nativeClient->Logout();

	EndHeartbeat();
}

void ClientBase::SendHeartbeat(Object ^obj)
{
	IntPtr tsPtr;
	try
	{
		String ^tsClient = System::DateTime::Now.ToString();
		tsPtr = (IntPtr)Marshal::StringToHGlobalAnsi(tsClient);
		_nativeClient->SendHeartbeat((char*)tsPtr.ToPointer());
	}
	finally
	{
		Marshal::FreeHGlobal(tsPtr);
	}
}

void ClientBase::BeginHeartbeat()
{
	TimerCallback^ tcb = gcnew TimerCallback(this, &ClientBase::SendHeartbeat);
	_heartTimer = gcnew Timer(tcb, nullptr, 10000, 60000);
}

void ClientBase::EndHeartbeat()
{
	if(_heartTimer != nullptr)
		_heartTimer->Change(Timeout::Infinite, Timeout::Infinite);
}

void ClientBase::AddPortfolio( PortfolioItem ^portfItem )
{
	if(this->IsConnected)
		_nativeClient->AddPortfolio(portfItem);
}

void ClientBase::AddPortfCollection( array<PortfolioItem ^> ^portfItems )
{
	if(this->IsConnected)
		_nativeClient->AddPortfolios(portfItems);
}

void ClientBase::PortfEnableStrategy( String ^portfId, bool isEnabled, int lastOrderId )
{
	if(!this->IsConnected)
		return;

	IntPtr pPortfIdAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(portfId);
		_nativeClient->PortfEnableStrategy((char*)pPortfIdAddress.ToPointer(), isEnabled, lastOrderId);
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
	}
}

void ClientBase::PortfTurnSwitches( String ^portfId, int triggerIndex, bool enabled )
{
	if(!this->IsConnected)
		return;

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
	if(!this->IsConnected)
		return;

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

void ClientBase::PortfModifyQuantity( String ^portfId, int perOpenQty, int perStartQty, int totalOpenLimit, int maxCancelQty, array<String^> ^timepoints )
{
	if(!this->IsConnected)
		return;

	IntPtr pPortfIdAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(portfId);
		vector<string> timepointsVector;
		for each(String ^tp in timepoints)
		{
			IntPtr tpPtr = (IntPtr)Marshal::StringToHGlobalAnsi(tp);
			timepointsVector.push_back((char*)tpPtr.ToPointer());
			Marshal::FreeHGlobal(tpPtr);
		}
		_nativeClient->PortfModifyQuantity((char*)pPortfIdAddress.ToPointer(), 
			perOpenQty, perStartQty, totalOpenLimit, maxCancelQty, timepointsVector);
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
	}
}

void ClientBase::PortfClosePosition( String ^pid, String ^mlOrder, String ^legRef )
{
	if(!this->IsConnected)
		return;

	IntPtr pPortfIdAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(pid);
		_nativeClient->PortfClosePosition((char*)pPortfIdAddress.ToPointer());
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
	}
}

void ClientBase::PortfOpenPosition( String ^pid, int quantity, bool isVirtual )
{
	if(!this->IsConnected)
		return;

	IntPtr pPortfIdAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(pid);
		_nativeClient->PortfOpenPosition((char*)pPortfIdAddress.ToPointer());
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
	}
}

void ClientBase::PortfSetPreferredLeg(String ^portfId, String ^legName)
{
	if (!this->IsConnected)
		return;

	IntPtr pPortfIdAddress;
	IntPtr pLegNameAddress;
	try
	{
		pPortfIdAddress = (IntPtr)Marshal::StringToHGlobalAnsi(portfId);
		pLegNameAddress = (IntPtr)Marshal::StringToHGlobalAnsi(legName);
		_nativeClient->PortfChangePreferredLeg(
			(char*)pPortfIdAddress.ToPointer(), (char*)pLegNameAddress.ToPointer());
	}
	finally
	{
		Marshal::FreeHGlobal(pPortfIdAddress);
		Marshal::FreeHGlobal(pLegNameAddress);
	}
}

}