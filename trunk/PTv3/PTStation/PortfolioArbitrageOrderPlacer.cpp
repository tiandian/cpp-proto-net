#include "StdAfx.h"
#include "PortfolioArbitrageOrderPlacer.h"


CPortfolioArbitrageOrderPlacer::CPortfolioArbitrageOrderPlacer(void)
{
}


CPortfolioArbitrageOrderPlacer::~CPortfolioArbitrageOrderPlacer(void)
{
}

void CPortfolioArbitrageOrderPlacer::BuildTemplateOrder()
{
    boost::shared_ptr<trade::MultiLegOrder> pMultiLegOrder(new trade::MultiLegOrder);

	pMultiLegOrder->set_quantity(m_pPortf->Quantity());
	pMultiLegOrder->set_portfolioid(m_pPortf->ID());
	pMultiLegOrder->set_offset(trade::ML_OF_OTHER);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");

	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	pMultiLegOrder->set_opendate(boost::gregorian::to_iso_string(d));
	pMultiLegOrder->set_reason(trade::SR_Trend);

    int legCount = m_pPortf->Count();
    assert( legCount > 1);
	
	for(int legIdx = 1; legIdx <= legCount; ++legIdx)
	{
        CLeg* leg = m_pPortf->GetLeg(legIdx);
		
		trade::Order* order = pMultiLegOrder->add_legs();
		order->set_brokerid(m_pOrderProcessor->BrokerId());
		order->set_investorid(m_pOrderProcessor->InvestorId());
		order->set_instrumentid(leg->Symbol());
		order->set_orderref("");

		order->set_orderpricetype(trade::LIMIT_PRICE);

		static char CombHedgeFlag[] = { static_cast<char>(trade::SPECULATION) };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

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

		order->set_preferred(leg->IsPreferred());
		
    }

    m_multiLegOrderTemplate = pMultiLegOrder;
}

void CPortfolioArbitrageOrderPlacer::OpenPosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp)
{
	Run(posiDirection, trade::OF_OPEN, pLmtPxArr, iPxSize, trigQuoteTimestamp):
}

void CPortfolioArbitrageOrderPlacer::ClosePosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp)
{
	Run(posiDirection, trade::OF_CLOSE_TODAY, pLmtPxArr, iPxSize, trigQuoteTimestamp):
}

void CPortfolioArbitrageOrderPlacer::Run(entity::PosiDirectionType posiDirection, trade::OffsetFlagType offset, trade::OF double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp)
{
	m_isWorking.store(true, boost::memory_order_release);

	m_posiDirection = posiDirection;
	entity::PosiDirectionType effectivePosiDirection = posiDirection;
	if(offset != trade::OF_OPEN)	// Closing position, revert
	{
		effectivePosiDirection = posiDirection == entity::LONG ? entity::SHORT : entity::LONG;
	}
	
    static trade::TradeDirectionType LONG_TRADE_SEQ[2] = {trade::BUY, trade::SELL};
    static trade::TradeDirectionType SHORT_TRADE_SEQ[2] = {trade::SELL, trade::BUY};

	int legCount = m_multiLegOrderTemplate->legs_size();
	assert(legCount == iPxSize);
	for(int i = 0; i < legCount; ++i)
    {
		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(i);
		
		// Set Offset
		char CombOffset[1];
		CombOffset[0] = offset;

		pOrd->set_comboffsetflag(std::string(CombOffset));
		
		// Set Direction
		if(effectivePosiDirection == entity::LONG)
		{
			// open long position
			pOrd->set_direction(LONG_TRADE_SEQ[i]);
		}
		else if(effectivePosiDirection == entity::SHORT)
		{
			pOrd->set_direction(SHORT_TRADE_SEQ[i]);
		}
		else
		{
			throw CUnexpectedPositionDirectionException();
		}
		
		// Set Limit Price
		pOrd->set_limitprice(pLmtPxArr[i]);
		
		// Change corresponding LegOrderPlacer
		CLegOrderPlacer* legOrdPlacer = GetLegOrderPlacer(pOrder->Symbol())
		if(legOrderPlacer != NULL)
		{
			legOrderPlacer->InputOrder().set_comboffsetflag(pOrd->comboffsetflag());
			legOrderPlacer->InputOrder().set_direction(pOrd->direction());
			legOrderPlacer->InputOrder().set_limitprice(pOrd->limitprice());
		}
	}
	
    m_triggingTimestamp = trigQuoteTimestamp;

    // Start fsm, fsm goes into Sending status
    boost::static_pointer_cast<OrderPlacerFsm>(m_fsm)->start();

    // And Sending the first leg
    Send();
}

CLegOrderPlacer* CPortfolioArbitrageOrderPlacer::GetLegOrderPlacer(const string& symbol)
{
	for(vector<LegOrderPlacerPtr>::iterator iter = m_legPlacers.begin();
                iter != m_legPlacers.end(); ++iter, ++seq)
    {
		if((*iter)->Symbol() == symbol)
		{
			return (*iter).get();
		}
    }
	
	return NULL;
}

bool CPortfolioArbitrageOrderPlacer::IsOpened()
{
	return false;
}

//*****  To be added to ArbitrageStrategy
void CArbitrageStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioArbitrageOrderPlacer* pOrderPlacer, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening )
{
	if(direction > entity::NET)
	{
		double lmtPrice[2];
		assert(pPortfolio->Count() > 1);
		if(direction == entity::LONG)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Ask();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Bid();
		}
		else if(direction == entity::SHORT)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Bid();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Ask();
		}
		
		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Open position @ %.2f - %.2f (%s)")
                        % GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1] % pQuote->update_time()));
						
		// TODO feed commment
		//pOrderPlacer->SetMlOrderStatus(openComment);
		pOrderPlacer->OpenPosition(direction, lmtPrice, 2, timestamp);
		m_side = direction;
		m_costDiff = lmtPrice[0] - lmtPrice[1];
		ResetForceOpen();
	}
}

void CArbitrageStrategy::ClosePosition( CPortfolioArbitrageOrderPlacer* pOrderPlacer, CPortfolio* pPortfolio, const string& comment )
{
	if(pOrderPlacer != NULL)
    {
        entity::PosiDirectionType direction = pOrderPlacer->PosiDirection();

		double lmtPrice[2];
		assert(pPortfolio->Count() > 1);
		if(direction == entity::LONG)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Bid();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Ask();
		}
		else if(direction == entity::SHORT)
		{
			CLeg* leg1 = pPortfolio->GetLeg(1);
			if(leg1 != NULL)
				lmtPrice[0] = leg1->Ask();
			CLeg* leg2 = pPortfolio->GetLeg(2);
			if(leg2 != NULL)
				lmtPrice[1] = leg2->Bid();
		}
		
        LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Close position @ %.2f - %.2f (%s)")
                        % GetPosiDirectionText(posiDirection) % lmtPrice[0] % lmtPrice[1]  % pQuote->update_time()));
						
		pOrderPlacer->ClosePosition(direction, lmtPrice, 2, timestamp);

        m_openAtBarIdx = 0; // reset open bar position
        ResetForceClose();
        pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s Æ½²Ö @ %.2f - %.2f")
                        % comment % GetPosiDirectionText(posiDirection, true) % lmtPrice[0] % lmtPrice[1]));

    }
}
