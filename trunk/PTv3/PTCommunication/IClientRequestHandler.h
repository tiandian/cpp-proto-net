#pragma once

#include "ServerLoginReturn.h"
#include "PortfolioUpdateItem.h"
#include "MultiLegOrder.h"
#include "Order.h"
#include "TradeUpdate.h"

using namespace System;
using namespace PTEntity;

namespace PTCommunication {

public interface class IClientRequestHandler
{
	void OnLoginReturned(LoginReturn ^loginReturn);
	void OnServerLoginReturned(ServerLoginReturn ^loginReturn);
	void OnPortfolioUpdate(PortfolioUpdateItem ^portfolioUpdateItem);
	void OnMultiLegOrderUpdate(MultiLegOrder ^multiLegOrder);
	void OnLegOrderUpdate(String ^portfId, String ^mlOrderId, String ^legOrdRef, Order ^legOrder);
	void OnTradeUpdate(TradeUpdate ^trade);
};

}