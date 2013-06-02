#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include <string>


const char* GetSumbitStatusText(trade::OrderSubmitStatusType submitStatus);

const char* GetStatusText(trade::OrderStatusType status);

void GetOrderUid( trade::InputOrder* pInputOrd, std::string& outOrdUid );
void GetOrderUid( trade::Order* pOrd, std::string& outOrdUid);