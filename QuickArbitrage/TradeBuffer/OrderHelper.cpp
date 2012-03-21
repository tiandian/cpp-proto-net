#include "StdAfx.h"
#include "OrderHelper.h"
#include "Portfolio.h"

#include <boost/foreach.hpp>

boost::shared_ptr<InputOrderVec> CreateOpenPositionOrders(CPortfolio* pPortfolio, AssignOrderRefCallback orderRefCallback)
{
	boost::shared_ptr<InputOrderVec> orders(new InputOrderVec);

	const LegVector& legs = pPortfolio->GetLegs();

	BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, legs)
	{
		boost::shared_ptr<protoc::InputOrder> order(new protoc::InputOrder);
		order->set_brokerid("0240");
		order->set_investorid("0240050002");
		order->set_instrumentid(leg->GetSymbol());
		
		if(orderRefCallback != NULL)
			order->set_orderref(orderRefCallback());

		order->set_orderpricetype(protoc::LIMIT_PRICE);

		order->set_direction(leg->GetSide() == protoc::LONG ? protoc::BUY : protoc::SELL);

		char CombOffset[] = { static_cast<char>(protoc::OF_OPEN) };
		order->set_comboffsetflag(std::string(CombOffset));
		
		char CombHedgeFlag[] = { static_cast<char>(protoc::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		order->set_limitprice(63000);
		order->set_volumetotaloriginal(leg->GetQuantity());
		order->set_timecondition(protoc::TC_GFD);

		order->set_volumecondition(protoc::VC_AV);
		order->set_minvolume(1);

		order->set_contingentcondition(protoc::IMMEDIATELY);
		order->set_forceclosereason(protoc::NOT_FORCE_CLOSE);
		order->set_isautosuspend(false);
		order->set_userforceclose(false);

		orders->push_back(order);
	}

	return orders;
}
