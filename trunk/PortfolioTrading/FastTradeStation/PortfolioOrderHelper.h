#pragma once

#include "Portfolio.h"
#include "entity/trade.pb.h"

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

trade::MultiLegOrder* BuildOpenPosiOrder(CPortfolio* portfolio, entity::PosiDirectionType direction, PlaceOrderContext* placeOrderCtx);

trade::MultiLegOrder* BuildClosePosiOrder(CPortfolio* portfolio, entity::PosiDirectionType portfDirection, const trade::MultiLegOrder* multilegOpenOrder, int quantity, PlaceOrderContext* placeOrderCtx);

trade::MultiLegOrder* BuildChangePosiOrder(CPortfolio* portfolio, const std::string& closeSymbol, entity::PosiDirectionType existingPosition, PlaceOrderContext* placeOrderCtx);

trade::MultiLegOrder* BuildScalperOrder(CPortfolio* portfolio, trade::PosiDirectionType posiDirection, double precedence, PlaceOrderContext* placeOrderCtx);

int GetInputOrders(trade::MultiLegOrder* multilegOrder, std::vector<boost::shared_ptr<trade::InputOrder>>* genInputOrders);

trade::InputOrder* BuildSingleOrder(const string& symbol, double limitPrice, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, PlaceOrderContext* placeOrderCtx);
