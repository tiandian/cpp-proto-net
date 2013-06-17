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
using namespace PTEntity;

namespace PTCommunication {

	public ref class ClientBase
	{
	private:
		CNatvieClient *_navtiveClient;
	
	public:
		ClientBase(IClientRequestHandler ^requestHandler)
		{
			_navtiveClient = new CNatvieClient(requestHandler);
		}
		~ClientBase()
		{
			this->!ClientBase();
		}
		!ClientBase()
		{
			if(_navtiveClient != 0)
			{
				delete _navtiveClient;
				_navtiveClient = 0;
			}
		}

		property bool IsConnected
		{
			bool get()
			{
				return _navtiveClient->logged();
			}
		}

		void SetPseudo(String ^pseudo);

		bool Connect(String ^host, int port);

		void ServerLogin(ServerType svrType, String ^address, String ^brokerId, String ^userId, String ^password)
		{

		}

		void AddPortfolio(PortfolioItem ^portfItem)
		{

		}

		void AddPortfCollection(array<PortfolioItem ^> ^portfItems)
		{

		}

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

		void PortfModifyQuantity(String ^portfId, int onceQty, int maxQty)
		{

		}

		void PortfSetPreferredLeg(String ^portfId, String ^legName)
		{

		}

		void PortfEnableStrategy(String ^portfId, bool isEnabled)
		{

		}

		void PortfTurnSwitches(String ^portfId, bool autoOpen, bool autoStopGain, bool autoStopLoss,
			bool autoTracking, bool enablePrefer)
		{

		}

		void PortfApplyStrategySettings(String ^portfId, String ^strategyName, array<Byte> ^strategyData)
		{

		}

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
	};
}
