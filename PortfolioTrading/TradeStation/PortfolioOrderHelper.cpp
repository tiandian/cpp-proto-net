#include "stdafx.h"
#include "PortfolioOrderHelper.h"


trade::MultiLegOrder* BuildOrder(CPortfolio* portfolio)
{
	trade::MultiLegOrder* pMultiLegOrder = new trade::MultiLegOrder;
	trade::Order* pOrder = pMultiLegOrder->add_legs();
	
	return pMultiLegOrder;
}