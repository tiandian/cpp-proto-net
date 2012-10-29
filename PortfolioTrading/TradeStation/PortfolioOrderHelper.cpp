#include "stdafx.h"
#include "PortfolioOrderHelper.h"
#include <boost/foreach.hpp>
#include <boost/date_time.hpp>


trade::MultiLegOrder* BuildOpenPosiOrder(CPortfolio* portfolio, PlaceOrderContext* placeOrderCtx)
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	string mOrderId;
	portfolio->NewOrderId(mOrderId);
	pMultiLegOrder->set_orderid(mOrderId);
	pMultiLegOrder->set_openorderid(mOrderId);
	pMultiLegOrder->set_quantity(placeOrderCtx->quantity);
	pMultiLegOrder->set_portfolioid(portfolio->ID());
	BOOST_FOREACH(LegPtr leg, portfolio->Legs())
	{
		trade::Order* order = pMultiLegOrder->add_legs();
		
		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		//order->set_orderref(NextOrderRef());
		
		entity::PosiDirectionType side = leg->Side();
		
		double limitPrice = 0;
		// in case wanna open position
		if(side == entity::LONG)
		{
			// open long position
			order->set_direction(trade::BUY);
			limitPrice = leg->Ask();
		}
		else if(side == entity::SHORT)
		{
			order->set_direction(trade::SELL);
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

		order->set_ordersubmitstatus(trade::INSERT_SUBMITTED);
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

	return trade::OF_CLOSE;
}

trade::MultiLegOrder* BuildClosePosiOrder(CPortfolio* portfolio, const trade::MultiLegOrder* multilegOpenOrder, PlaceOrderContext* placeOrderCtx)
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	string mOrderId;
	portfolio->NewOrderId(mOrderId);
	pMultiLegOrder->set_orderid(mOrderId);
	pMultiLegOrder->set_openorderid(multilegOpenOrder->orderid());
	pMultiLegOrder->set_quantity(multilegOpenOrder->quantity());
	pMultiLegOrder->set_portfolioid(portfolio->ID());
	BOOST_FOREACH(LegPtr leg, portfolio->Legs())
	{
		trade::Order* order = pMultiLegOrder->add_legs();

		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		//order->set_orderref(NextOrderRef());

		entity::PosiDirectionType side = leg->Side();

		double limitPrice = 0;
		// in case wanna open position
		if(side == entity::LONG)
		{
			// open long position
			order->set_direction(trade::SELL);
			limitPrice = leg->Bid();
		}
		else if(side == entity::SHORT)
		{
			order->set_direction(trade::BUY);
			limitPrice = leg->Ask();
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
		CombOffset[0] = GetCloseFlag(order->instrumentid(), multilegOpenOrder->opendate());

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

		order->set_ordersubmitstatus(trade::INSERT_SUBMITTED);
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
	BOOST_FOREACH(LegPtr leg, portfolio->Legs())
	{
		trade::Order* order = pMultiLegOrder->add_legs();

		bool isClosingLeg = leg->Symbol() == closeSymbol;

		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		
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

		order->set_ordersubmitstatus(trade::INSERT_SUBMITTED);
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
