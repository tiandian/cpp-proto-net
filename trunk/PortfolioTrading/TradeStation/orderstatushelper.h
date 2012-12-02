#pragma once

#include "../Entity/gen/cpp/trade.pb.h"

const char* GetSumbitStatusText(trade::OrderSubmitStatusType submitStatus);

const char* GetStatusText(trade::OrderStatusType status);

