#pragma once

#include "entity/trade.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class MultiLegOrder
{
public:
	MultiLegOrder(trade::MultiLegOrder* pEntity);
};

}

