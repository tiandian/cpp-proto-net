#pragma once

#include "entity/trade.pb.h"

using namespace System;

namespace PTEntity {

public ref class Order
{
public:
	Order(const trade::Order* pEntityOrd){}
};

}

