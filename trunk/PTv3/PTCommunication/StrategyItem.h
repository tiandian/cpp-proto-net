#pragma once

#include "entity/message.pb.h"

using namespace System;
using namespace System::Collections::Generic;

namespace PTEntity {

public ref class StrategyItem
{
public:
	StrategyItem(void);

	void To(entity::StrategyItem* pNativeStrategyItem);
};

}