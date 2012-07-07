#include "StdAfx.h"
#include "TradeAgent.h"


CTradeAgent::CTradeAgent(void)
{
}


CTradeAgent::~CTradeAgent(void)
{
}

boost::tuple<bool, string> CTradeAgent::Open( const string& address, const string& streamDir )
{
	return boost::make_tuple(false, "");
}

void CTradeAgent::Close()
{

}

boost::tuple<bool, string> CTradeAgent::Login( const string& brokerId, const string& userId, const string& password )
{
	return boost::make_tuple(false, "");
}

void CTradeAgent::Logout()
{

}
