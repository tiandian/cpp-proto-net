// PTCommunication.h

#pragma once

#include "NatvieClient.h"
#include "Enums.h"
#include "PortfolioItem.h"
#include "MultiLegOrder.h"
#include "Order.h"
#include "AccountInfo.h"
#include "OperationResult.h"
#include "SymbolInfo.h"
#include "IClientRequestHandler.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace PTEntity;

namespace PTCommunication {

	public ref class ClientBase
	{
	private:
		CNatvieClient *_nativeClient;
		Timer ^_heartTimer;

	public:
		ClientBase(IClientRequestHandler ^requestHandler)
		{
			_nativeClient = new CNatvieClient(requestHandler);
		}
		~ClientBase()
		{
			this->!ClientBase();
		}
		!ClientBase()
		{
			if(_nativeClient != 0)
			{
				delete _nativeClient;
				_nativeClient = 0;
			}
		}

		property bool IsConnected
		{
			bool get()
			{
				return _nativeClient->logged();
			}
		}

		void Disconnect();

		void SetPseudo(String ^pseudo);

		bool Connect(String ^host, int port);

		void ServerLogin(ServerType svrType, String ^address, String ^brokerId, String ^investorId, String ^password);

		void ServerLogout(ServerType svrType);

		void AddPortfolio(PortfolioItem ^portfItem);

		void AddPortfCollection(array<PortfolioItem ^> ^portfItems);

		void RemovePortf(String ^portfId)
		{

		}

		void PortfOpenPosition(String ^pid, int quantity, bool isVirtual)
		{

		}

		void SwitchPosition(String ^pid, int quantity)
		{

		}

		String^ PortfClosePosition(MultiLegOrder ^mlOrder, String ^legRef)
		{
			return gcnew String("");
		}

		void CancelOrder(Order ^legOrder)
		{

		}

		void PortfModifyQuantity(String ^portfId, int perOpenQty, int perStartQty, int totalOpenLimit, int maxCancelQty);

		void PortfSetPreferredLeg(String ^portfId, String ^legName)
		{

		}

		void PortfEnableStrategy(String ^portfId, bool isEnabled, int lastOrderId);

		void PortfTurnSwitches(String ^portfId, int triggerIndex, bool enabled);

		void PortfApplyStrategySettings(String ^portfId, StrategyItem ^strategyItem);

		AccountInfo^ QueryAccountInfo()
		{
			return nullptr;
		}

		void QueryPositionDetails(String ^symbol)
		{

		}

		OperationResult^ ManualCloseOrder(String ^symbol, TradeDirectionType direction,
			DateTime ^openDate, int quantity)
		{
			return nullptr;
		}

		SymbolInfo^ QuerySymbolInfo(String ^symbol)
		{
			return nullptr;
		}

		void BeginHeartbeat();
		void EndHeartbeat();

	private:
		void SendHeartbeat(Object ^obj);
	};
}
