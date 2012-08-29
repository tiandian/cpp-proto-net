#pragma once

#include "Portfolio.h"
#include "../Entity/gen/cpp/trade.pb.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

class PlaceOrderContext
{
public:
	std::string brokerId;
	std::string investorId;
	trade::OrderPriceTypeType orderPriceType;
	entity::LimitPriceType limitPriceType;
	int quantity;
};

trade::MultiLegOrder* BuildOpenPosiOrder(CPortfolio* portfolio, PlaceOrderContext* placeOrderCtx);

int GetInputOrders(trade::MultiLegOrder* multilegOrder, std::vector<boost::shared_ptr<trade::InputOrder>>* genInputOrders);
