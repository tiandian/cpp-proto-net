#pragma once

#include "Portfolio.h"
#include "../Entity/gen/cpp/trade.pb.h"

trade::MultiLegOrder* BuildOrder(CPortfolio* portfolio);
