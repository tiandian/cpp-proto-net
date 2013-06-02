#include "StdAfx.h"
#include "orderhelper.h"

#include <boost/format.hpp>

const char* SUBMIT_STATUS_TEXT[] = {"NOT_SUBMITTED", "INSERT_SUBMITTED", "CANCEL_SUBMITTED",
	"MODIFY_SUBMITTED", "ACCEPTED", "INSERT_REJECTED", "CANCEL_REJECTED", "MODIFY_REJECTED"};

const char* STATUS_TEXT[] = {"ALL_TRADED", "PART_TRADED_QUEUEING", "PART_TRADED_NOT_QUEUEING",
	"NO_TRADE_QUEUEING", "NO_TRADE_NOT_QUEUEING", "ORDER_CANCELED", "STATUS_UNKNOWN",
	"ORDER_NOT_TOUCHED", "ORDER_TOUCHED"};


const char* GetSumbitStatusText( trade::OrderSubmitStatusType submitStatus )
{
	return SUBMIT_STATUS_TEXT[submitStatus - trade::NOT_SUBMITTED];
}

const char* GetStatusText( trade::OrderStatusType status )
{
	if(status >= trade::STATUS_UNKNOWN)
	{
		return STATUS_TEXT[status - trade::STATUS_UNKNOWN + 6];
	}

	return STATUS_TEXT[status - trade::ALL_TRADED];
}

const char* GetOrderDirectionText(trade::TradeDirectionType direction)
{
	switch(direction)
	{
	case trade::BUY:
		return "B";
	case trade::SELL:
		return "S";
	default:
		return "";
	}
}

void GetOrderUid( trade::InputOrder* pInputOrd, std::string& outOrdUid )
{
	outOrdUid = boost::str(boost::format("%s-%s")
		% GetOrderDirectionText(pInputOrd->direction())
		% pInputOrd->instrumentid());
}

void GetOrderUid( trade::Order* pOrd, std::string& outOrdUid )
{
	outOrdUid = boost::str(boost::format("%s-%s")
		% GetOrderDirectionText(pOrd->direction())
		% pOrd->instrumentid());
}
