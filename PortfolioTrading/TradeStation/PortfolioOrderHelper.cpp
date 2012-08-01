#include "stdafx.h"
#include "PortfolioOrderHelper.h"
#include <boost/foreach.hpp>


trade::MultiLegOrder* BuildOrder(CPortfolio* portfolio, PlaceOrderContext* placeOrderCtx)
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	BOOST_FOREACH(LegPtr leg, portfolio->Legs())
	{
		trade::Order* order = pMultiLegOrder->add_legs();

		order->set_brokerid(placeOrderCtx->brokerId);
		order->set_investorid(placeOrderCtx->investorId);
		order->set_instrumentid(leg->Symbol());
		//order->set_orderref(NextOrderRef());
		entity::LegStatus status = leg->Item()->status();
		entity::PosiDirectionType side = leg->Item()->side();
		static char CombOffset[1];
		if(status == entity::UNOPENED)
		{
			// in case wanna open position
			if(side == entity::LONG)
			{
				// open long position
				order->set_direction(trade::BUY);
			}
			else if(side == entity::SHORT)
			{
				order->set_direction(trade::SELL);
			}
			else
			{
				throw std::exception("unexpected leg side");
			}
			order->set_orderpricetype(placeOrderCtx->orderPriceType);
			order->set_limitprice(placeOrderCtx->limitPrice);
			CombOffset[0] = trade::OF_OPEN;
		}
		else if(status == entity::OPENED)
		{
			// in case wanna close position
			if(side == entity::LONG)
			{
				order->set_direction(trade::SELL);
			}
			else if(side == entity::SHORT)
			{
				order->set_direction(trade::BUY);
			}
			else
			{
				throw std::exception("unexpected leg side");
			}
			order->set_orderpricetype(placeOrderCtx->orderPriceType);
			order->set_limitprice(placeOrderCtx->limitPrice);
			CombOffset[0] = trade::OF_CLOSE_TODAY;
		}
		else
		{
			assert(false);	// leg status is incorret
		}

		//order->set_orderpricetype(protoc::LIMIT_PRICE);

		//order->set_direction(leg->GetSide() == protoc::LONG ? protoc::BUY : protoc::SELL);

		// 	char CombOffset[] = { static_cast<char>(protoc::OF_OPEN) };
		order->set_comboffsetflag(std::string(CombOffset));

		static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		// 	order->set_limitprice(0);
		int qty = pMultiLegOrder->quantity() * leg->Item()->ratio();
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
	}
	
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

		genInputOrders->push_back(inputOrder);
	}

	return genInputOrders->size();
}