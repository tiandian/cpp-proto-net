#include "PortfolioScalperOrderPlacer.h"
#include "Portfolio.h"
#include "InputOrder.h"
#include "OrderProcessor2.h"
#include "BuildOrderException.h"

CPortfolioScalperOrderPlacer::CPortfolioScalperOrderPlacer(CPortfolio* pPortf, COrderProcessor2* pOrderProc)
	:CPortfolioOrderPlacer(pPortf, pOrderProc)
{
}


CPortfolioScalperOrderPlacer::~CPortfolioScalperOrderPlacer(void)
{
}

void CPortfolioScalperOrderPlacer::BuildTemplateOrder()
{
	static trade::OffsetFlagType OFFSET_FLAGS[2] = 
				{trade::OF_OPEN, trade::OF_CLOSE_TODAY};

	boost::shared_ptr<trade::MultiLegOrder> pMultiLegOrder(new trade::MultiLegOrder);
	
	pMultiLegOrder->set_quantity(m_pPortf->Quantity());
	pMultiLegOrder->set_portfolioid(m_pPortf->ID());
	pMultiLegOrder->set_offset(trade::ML_OF_OTHER);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");

	assert(m_pPortf->Count() == 1);

	CLeg* leg = m_pPortf->GetLeg(1);

	for(int i = 0; i < 2; ++i)
	{
		trade::Order* order = pMultiLegOrder->add_legs();
		order->set_brokerid(m_pOrderProcessor->BrokerId());
		order->set_investorid(m_pOrderProcessor->InvestorId());
		order->set_instrumentid(leg->Symbol());
		order->set_orderref("");

		/*
		double limitPrice = 0;
		// in case wanna open position
		if(posiDirection == trade::LONG)
		{
			// open long position
			order->set_direction(LONG_TRADE_SEQ[i]);
		}
		else if(posiDirection == trade::SHORT)
		{
			order->set_direction(SHORT_TRADE_SEQ[i]);
		}
		else
		{
			throw CUnexpectedPositionDirectionException();
		}
		
		limitPrice = order->direction() == trade::BUY ?
			(leg->Bid() + precedence) : (leg->Ask() - precedence);
		*/

		order->set_orderpricetype(trade::LIMIT_PRICE);
		//order->set_limitprice(limitPrice);

		static char CombOffset[1];
		CombOffset[0] = OFFSET_FLAGS[i];

		order->set_comboffsetflag(std::string(CombOffset));

		static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		// 	order->set_limitprice(0);
		int qty = pMultiLegOrder->quantity() * leg->Ratio();
		order->set_volumetotaloriginal(qty);

		order->set_timecondition(trade::TC_GFD);
		
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

	m_multiLegOrderTemplate = pMultiLegOrder;
}

void CPortfolioScalperOrderPlacer::SetDirection( trade::PosiDirectionType posiDirection )
{
	static trade::TradeDirectionType LONG_TRADE_SEQ[2] = {trade::BUY, trade::SELL};
	static trade::TradeDirectionType SHORT_TRADE_SEQ[2] = {trade::SELL, trade::BUY};

	for(int i = 0; i < m_multiLegOrderTemplate->legs_size(); ++i)
	{
		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(i);

		// in case wanna open position
		if(posiDirection == trade::LONG)
		{
			// open long position
			pOrd->set_direction(LONG_TRADE_SEQ[i]);
			m_inputOrders[i]->set_direction(LONG_TRADE_SEQ[i]);
		}
		else if(posiDirection == trade::SHORT)
		{
			pOrd->set_direction(SHORT_TRADE_SEQ[i]);
			m_inputOrders[i]->set_direction(SHORT_TRADE_SEQ[i]);
		}
		else
		{
			throw CUnexpectedPositionDirectionException();
		}
	}
}

void CPortfolioScalperOrderPlacer::SetLimitPrice(double* pLmtPxArr, int iPxSize)
{
	for(int i = 0; i < iPxSize; ++i)
	{
		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(i);
		double lmtPx = pLmtPxArr[i];
		pOrd->set_limitprice(lmtPx);
		m_inputOrders[i]->set_limitprice(lmtPx);
	}
}
