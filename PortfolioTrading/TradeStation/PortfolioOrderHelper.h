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

trade::MultiLegOrder* BuildClosePosiOrder(CPortfolio* portfolio, const trade::MultiLegOrder* multilegOpenOrder, int quantity, PlaceOrderContext* placeOrderCtx);

trade::MultiLegOrder* BuildChangePosiOrder(CPortfolio* portfolio, const std::string& closeSymbol, entity::PosiDirectionType existingPosition, PlaceOrderContext* placeOrderCtx);

int GetInputOrders(trade::MultiLegOrder* multilegOrder, std::vector<boost::shared_ptr<trade::InputOrder>>* genInputOrders);

trade::InputOrder* BuildCloseOrder(const string& symbol, double limitPrice, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, PlaceOrderContext* placeOrderCtx);
