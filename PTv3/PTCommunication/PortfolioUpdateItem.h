#pragma once

#include "entity/message.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class PortfolioUpdateItem
{
public:
	PortfolioUpdateItem(entity::PortfolioUpdateItem* pEntity);
};

}

