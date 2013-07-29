#pragma once

#include "Enums.h"
#include "Order.h"
#include "entity/trade.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class MultiLegOrder
{
public:
	MultiLegOrder(const trade::MultiLegOrder* pEntity);

	property String ^ OrderId;
	property String ^ PortfolioId;
	property int Quantity;

	property array<Order^>^ Legs;
	property String ^ OpenOrderId;
	property String ^ OpenDate;

	property SubmitReason Reason;
	property bool HasWarn;
	property String ^ StatusMsg;
	property MlOrderOffset Offset;
};

}

