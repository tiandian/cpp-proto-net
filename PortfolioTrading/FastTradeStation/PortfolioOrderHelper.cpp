#include "PortfolioOrderHelper.h"
#include <boost/foreach.hpp>
#include <boost/date_time.hpp>


trade::MultiLegOrder* BuildOpenPosiOrder(CPortfolio* portfolio, entity::PosiDirectionType direction, PlaceOrderContext* placeOrderCtx)
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	string mOrderId;
	portfolio->NewOrderId(mOrderId);
	pMultiLegOrder->set_orderid(mOrderId);
	pMultiLegOrder->set_openorderid(mOrderId);
	pMultiLegOrder->set_quantity(placeOrderCtx->quantity);
	pMultiLegOrder->set_portfolioid(portfolio->ID());
	pMultiLegOrder->set_offset(trade::ML_OF_OPEN);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");
	BOOST_FOREACH(LegPtr leg, portfolio->Legs())
	{
		trade::Order* order = pMultiLegOrder->add_legs();
		
		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		order->set_orderref("");
		
		entity::PosiDirectionType side = leg->Side();
		
		double limitPrice = 0;
		trade::TradeDirectionType legDirection;
		// in case wanna open position
		if(direction == entity::LONG)
		{
			legDirection = side == entity::LONG ? trade::BUY : trade::SELL; 
		}
		else
		{
			legDirection = side == entity::LONG ? trade::SELL : trade::BUY;
		}
		order->set_direction(legDirection);
		if(legDirection == trade::BUY)
		{
			// open long position
			limitPrice = leg->Ask();
		}
		else if(legDirection == trade::SELL)
		{
			limitPrice = leg->Bid();
		}
		else
		{
			throw std::exception("unexpected leg side");
		}

		if(placeOrderCtx->limitPriceType == entity::Last)
			limitPrice = leg->Last();

		order->set_orderpricetype(placeOrderCtx->orderPriceType);
		order->set_limitprice(limitPrice);

		//order->set_orderpricetype(protoc::LIMIT_PRICE);

		//order->set_direction(leg->GetSide() == protoc::LONG ? protoc::BUY : protoc::SELL);

		//char CombOffset[] = { static_cast<char>(protoc::OF_OPEN) };
		static char CombOffset[1];
		CombOffset[0] = trade::OF_OPEN;

		order->set_comboffsetflag(std::string(CombOffset));

		static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		// 	order->set_limitprice(0);
		int qty = pMultiLegOrder->quantity() * leg->Ratio();
		order->set_volumetotaloriginal(qty);

		if(order->orderpricetype() == trade::ANY_PRICE)
		{
			// IOC needed for market price
			order->set_timecondition(trade::TC_IOC);
		}
		else
		{
			order->set_timecondition(trade::TC_GFD);
		}

		order->set_volumecondition(trade::VC_AV);
		order->set_minvolume(1);

		order->set_contingentcondition(trade::IMMEDIATELY);
		order->set_forceclosereason(trade::NOT_FORCE_CLOSE);
		order->set_isautosuspend(false);
		order->set_userforceclose(false);

		order->set_ordersubmitstatus(trade::NOT_SUBMITTED);
		order->set_orderstatus(trade::STATUS_UNKNOWN);

		order->set_preferred(leg->IsPreferred());
	}
	
	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	pMultiLegOrder->set_opendate(boost::gregorian::to_iso_string(d));

	return pMultiLegOrder;
}

trade::OffsetFlagType GetCloseFlag(const string& symbol, const string& openDate)
{
	int pos = symbol.find("cu");
	if(pos == 0)
	{
		boost::gregorian::date od = boost::gregorian::from_undelimited_string(openDate);
		boost::gregorian::date cd = boost::gregorian::day_clock::local_day();
		if(cd > od)
			return trade::OF_CLOSE_YESTERDAY;
		else
			return trade::OF_CLOSE_TODAY;
	}

	return trade::OF_CLOSE_TODAY;
}

trade::MultiLegOrder* BuildClosePosiOrder(CPortfolio* portfolio, entity::PosiDirectionType portfDirection, const trade::MultiLegOrder* multilegOpenOrder, int quantity, PlaceOrderContext* placeOrderCtx)
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	string mOrderId;
	portfolio->NewOrderId(mOrderId);
	pMultiLegOrder->set_orderid(mOrderId);

	if(multilegOpenOrder != NULL)
		pMultiLegOrder->set_openorderid(multilegOpenOrder->orderid());
	else
		pMultiLegOrder->set_openorderid("");
	
	pMultiLegOrder->set_quantity(quantity);
	pMultiLegOrder->set_portfolioid(portfolio->ID());
	pMultiLegOrder->set_offset(trade::ML_OF_CLOSE);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");
	BOOST_FOREACH(LegPtr leg, portfolio->Legs())
	{
		trade::Order* order = pMultiLegOrder->add_legs();

		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		order->set_orderref("");

		entity::PosiDirectionType side = leg->Side();

		double limitPrice = 0;
		trade::TradeDirectionType legDirection;
		// in case wanna open position
		if(portfDirection == entity::LONG)
		{
			legDirection = side == entity::LONG ? trade::SELL : trade::BUY; 
		}
		else
		{
			legDirection = side == entity::LONG ? trade::BUY : trade::SELL;
		}
		order->set_direction(legDirection);

		if(legDirection == trade::BUY)
		{
			// open long position
			limitPrice = leg->Ask();
		}
		else if(legDirection == trade::SELL)
		{
			limitPrice = leg->Bid();
		}
		else
		{
			throw std::exception("unexpected leg side");
		}

		if(placeOrderCtx->limitPriceType == entity::Last)
			limitPrice = leg->Last();

		order->set_orderpricetype(placeOrderCtx->orderPriceType);
		order->set_limitprice(limitPrice);

		static char CombOffset[1];
		if(multilegOpenOrder != NULL)
			CombOffset[0] = GetCloseFlag(order->instrumentid(), multilegOpenOrder->opendate());
		else
			CombOffset[0] = trade::OF_CLOSE_TODAY;

		order->set_comboffsetflag(std::string(CombOffset));

		static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		int qty = pMultiLegOrder->quantity() * leg->Ratio();
		order->set_volumetotaloriginal(qty);

		if(order->orderpricetype() == trade::ANY_PRICE)
		{
			// IOC needed for market price
			order->set_timecondition(trade::TC_IOC);
		}
		else
		{
			order->set_timecondition(trade::TC_GFD);
		}

		order->set_volumecondition(trade::VC_AV);
		order->set_minvolume(1);

		order->set_contingentcondition(trade::IMMEDIATELY);
		order->set_forceclosereason(trade::NOT_FORCE_CLOSE);
		order->set_isautosuspend(false);
		order->set_userforceclose(false);

		order->set_ordersubmitstatus(trade::NOT_SUBMITTED);
		order->set_orderstatus(trade::STATUS_UNKNOWN);

		order->set_preferred(leg->IsPreferred());
	}

	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	pMultiLegOrder->set_opendate(boost::gregorian::to_iso_string(d));

	return pMultiLegOrder;
}

trade::MultiLegOrder* BuildChangePosiOrder(CPortfolio* portfolio, 
	const std::string& closeSymbol, entity::PosiDirectionType existingPosition,
	PlaceOrderContext* placeOrderCtx)
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	string mOrderId;
	portfolio->NewOrderId(mOrderId);
	pMultiLegOrder->set_orderid(mOrderId);
	pMultiLegOrder->set_openorderid(mOrderId);
	pMultiLegOrder->set_quantity(placeOrderCtx->quantity);
	pMultiLegOrder->set_portfolioid(portfolio->ID());
	pMultiLegOrder->set_offset(trade::ML_OF_OTHER);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");
	BOOST_FOREACH(LegPtr leg, portfolio->Legs())
	{
		trade::Order* order = pMultiLegOrder->add_legs();

		bool isClosingLeg = leg->Symbol() == closeSymbol;

		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		order->set_orderref("");
		
		entity::PosiDirectionType side = leg->Side();

		double limitPrice = 0;
		// in case wanna open position
		if(existingPosition == entity::LONG)
		{
			// open long position
			order->set_direction(isClosingLeg ? trade::SELL : trade::BUY);
			limitPrice = isClosingLeg ? leg->Bid() : leg->Ask();
		}
		else if(existingPosition == entity::SHORT)
		{
			order->set_direction(isClosingLeg ? trade::BUY : trade::SELL);
			limitPrice = isClosingLeg ? leg->Ask() : leg->Bid();
		}
		else
		{
			throw std::exception("unexpected leg side");
		}

		if(placeOrderCtx->limitPriceType == entity::Last)
			limitPrice = leg->Last();

		order->set_orderpricetype(placeOrderCtx->orderPriceType);
		order->set_limitprice(limitPrice);

		static char CombOffset[1];
		CombOffset[0] = isClosingLeg ? trade::OF_CLOSE_TODAY : trade::OF_OPEN;
		
		order->set_comboffsetflag(std::string(CombOffset));

		static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		// 	order->set_limitprice(0);
		int qty = pMultiLegOrder->quantity() * leg->Ratio();
		order->set_volumetotaloriginal(qty);

		if(order->orderpricetype() == trade::ANY_PRICE)
		{
			// IOC needed for market price
			order->set_timecondition(trade::TC_IOC);
		}
		else
		{
			order->set_timecondition(trade::TC_GFD);
		}

		order->set_volumecondition(trade::VC_AV);
		order->set_minvolume(1);

		order->set_contingentcondition(trade::IMMEDIATELY);
		order->set_forceclosereason(trade::NOT_FORCE_CLOSE);
		order->set_isautosuspend(false);
		order->set_userforceclose(false);

		order->set_ordersubmitstatus(trade::NOT_SUBMITTED);
		order->set_orderstatus(trade::STATUS_UNKNOWN);

		order->set_preferred(leg->IsPreferred());
	}

	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	pMultiLegOrder->set_opendate(boost::gregorian::to_iso_string(d));

	return pMultiLegOrder;
}

int GetInputOrders(trade::MultiLegOrder* multilegOrder, std::vector<boost::shared_ptr<trade::InputOrder>>* genInputOrders)
{
	assert(genInputOrders != NULL);

	BOOST_FOREACH(const trade::Order& o, multilegOrder->legs())
	{
		boost::shared_ptr<trade::InputOrder> inputOrder(new trade::InputOrder);

		inputOrder->set_brokerid(o.brokerid());
		inputOrder->set_investorid(o.investorid());
		inputOrder->set_instrumentid(o.instrumentid());
		inputOrder->set_orderref(o.orderref());
		inputOrder->set_userid(o.userid());
		inputOrder->set_orderpricetype(o.orderpricetype());
		inputOrder->set_direction(o.direction());
		inputOrder->set_combhedgeflag(o.combhedgeflag());
		inputOrder->set_comboffsetflag(o.comboffsetflag());
		inputOrder->set_limitprice(o.limitprice());
		inputOrder->set_volumetotaloriginal(o.volumetotaloriginal());
		inputOrder->set_timecondition(o.timecondition());
		inputOrder->set_gtddate(o.gtddate());
		inputOrder->set_volumecondition(	o.volumecondition());
		inputOrder->set_minvolume(o.minvolume());
		inputOrder->set_contingentcondition(	o.contingentcondition());
		inputOrder->set_stopprice(o.stopprice());
		inputOrder->set_forceclosereason(o.forceclosereason());
		inputOrder->set_isautosuspend(o.isautosuspend());
		inputOrder->set_businessunit(o.businessunit());
		inputOrder->set_requestid(o.requestid());
		inputOrder->set_userforceclose(o.userforceclose());

		if(o.preferred())	// always put preferred order at front
			genInputOrders->insert(genInputOrders->begin(), inputOrder);
		else
			genInputOrders->push_back(inputOrder);
	}

	return genInputOrders->size();
}

trade::InputOrder* BuildSingleOrder(const string& symbol, double limitPrice, 
	trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag,
	PlaceOrderContext* placeOrderCtx)
{
	trade::InputOrder* order = new trade::InputOrder;
	order->set_brokerid(placeOrderCtx->brokerId);
	order->set_investorid(placeOrderCtx->investorId);
	order->set_instrumentid(symbol);

	order->set_orderref("");
	order->set_direction(direction);

	static char CombOffset[1];
	CombOffset[0] = offsetFlag;
	order->set_comboffsetflag(std::string(CombOffset));

	order->set_limitprice(limitPrice);
	order->set_orderpricetype(trade::LIMIT_PRICE);

	static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
	order->set_combhedgeflag(std::string(CombHedgeFlag));

	order->set_volumetotaloriginal(placeOrderCtx->quantity);
	order->set_timecondition(trade::TC_GFD);
	
	order->set_volumecondition(trade::VC_AV);
	order->set_minvolume(1);

	order->set_contingentcondition(trade::IMMEDIATELY);
	order->set_forceclosereason(trade::NOT_FORCE_CLOSE);
	order->set_isautosuspend(false);
	order->set_userforceclose(false);

	return order;
}

trade::OffsetFlagType OFFSET_FLAGS[2] = {trade::OF_OPEN, trade::OF_CLOSE_TODAY};
trade::TradeDirectionType LONG_TRADE_SEQ[2] = {trade::BUY, trade::SELL};
trade::TradeDirectionType SHORT_TRADE_SEQ[2] = {trade::SELL, trade::BUY};

trade::MultiLegOrder* BuildScalperOrder( CPortfolio* portfolio, trade::PosiDirectionType posiDirection, double precedence, PlaceOrderContext* placeOrderCtx )
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	string mOrderId;
	portfolio->NewOrderId(mOrderId);
	pMultiLegOrder->set_orderid(mOrderId);
	pMultiLegOrder->set_openorderid(mOrderId);
	pMultiLegOrder->set_quantity(placeOrderCtx->quantity);
	pMultiLegOrder->set_portfolioid(portfolio->ID());
	pMultiLegOrder->set_offset(trade::ML_OF_OTHER);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");

	assert(portfolio->Count() == 1);

	CLeg* leg = portfolio->GetLeg(1);

	for(int i = 0; i < 2; ++i)
	{
		trade::Order* order = pMultiLegOrder->add_legs();
		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		order->set_orderref("");
	
		double limitPrice = 0;
		// in case wanna open position
		if(posiDirection == entity::LONG)
		{
			// open long position
			order->set_direction(LONG_TRADE_SEQ[i]);
		}
		else if(posiDirection == entity::SHORT)
		{
			order->set_direction(SHORT_TRADE_SEQ[i]);
		}
		else
		{
			throw std::exception("unexpected position direction");
		}

		limitPrice = order->direction() == trade::BUY ?
			(leg->Bid() + precedence) : (leg->Ask() - precedence);

		order->set_orderpricetype(placeOrderCtx->orderPriceType);
		order->set_limitprice(limitPrice);

		static char CombOffset[1];
		CombOffset[0] = OFFSET_FLAGS[i];

		order->set_comboffsetflag(std::string(CombOffset));

		static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		// 	order->set_limitprice(0);
		int qty = pMultiLegOrder->quantity() * leg->Ratio();
		order->set_volumetotaloriginal(qty);

		if(order->orderpricetype() == trade::ANY_PRICE)
		{
			// IOC needed for market price
			order->set_timecondition(trade::TC_IOC);
		}
		else
		{
			order->set_timecondition(trade::TC_GFD);
		}

		order->set_volumecondition(trade::VC_AV);
		order->set_minvolume(1);

		order->set_contingentcondition(trade::IMMEDIATELY);
		order->set_forceclosereason(trade::NOT_FORCE_CLOSE);
		order->set_isautosuspend(false);
		order->set_userforceclose(false);

		order->set_ordersubmitstatus(trade::NOT_SUBMITTED);
		order->set_orderstatus(trade::STATUS_UNKNOWN);

		order->set_preferred(i == 0);
	}

	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	pMultiLegOrder->set_opendate(boost::gregorian::to_iso_string(d));

	return pMultiLegOrder;
}
