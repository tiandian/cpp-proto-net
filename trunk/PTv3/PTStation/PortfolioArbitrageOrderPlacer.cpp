#include "StdAfx.h"
#include "PortfolioArbitrageOrderPlacer.h"
#include "Portfolio.h"
#include "BuildOrderException.h"
#include "OrderProcessor.h"

#include <boost/date_time.hpp>

CPortfolioArbitrageOrderPlacer::CPortfolioArbitrageOrderPlacer(void)
	: m_openedPosition(false)
	, m_openingPosition(false)
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
	//pMultiLegOrder->set_offset(trade::ML_OF_OTHER);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");

	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	pMultiLegOrder->set_opendate(boost::gregorian::to_iso_string(d));
	pMultiLegOrder->set_reason(trade::SR_Manual);

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

void CPortfolioArbitrageOrderPlacer::OpenPosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, trade::SubmitReason reason)
{
	m_multiLegOrderTemplate->set_reason(reason);
	m_multiLegOrderTemplate->set_offset(trade::ML_OF_OPEN);
	Run(posiDirection, trade::OF_OPEN, pLmtPxArr, iPxSize, trigQuoteTimestamp, NULL);
	m_openingPosition = true;
	m_lastOpenOrderId = m_multiLegOrderTemplate->orderid();
}

void CPortfolioArbitrageOrderPlacer::ClosePosition(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, trade::SubmitReason reason)
{
	m_multiLegOrderTemplate->set_reason(reason);
	m_multiLegOrderTemplate->set_offset(trade::ML_OF_CLOSE);
	Run(posiDirection, trade::OF_CLOSE_TODAY, pLmtPxArr, iPxSize, trigQuoteTimestamp, m_lastOpenOrderId.c_str());
	m_openingPosition = false;
}

void CPortfolioArbitrageOrderPlacer::Run(entity::PosiDirectionType posiDirection, trade::OffsetFlagType offset, double* pLmtPxArr, int iPxSize, const boost::chrono::steady_clock::time_point& trigQuoteTimestamp, const char* openOrderId)
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
		char CombOffset[2] = { 0, 0 };
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
		CLegOrderPlacer* legOrdPlacer = GetLegOrderPlacer(pOrd->instrumentid());
		if(legOrdPlacer != NULL)
		{
			legOrdPlacer->InputOrder().set_comboffsetflag(pOrd->comboffsetflag());
			legOrdPlacer->InputOrder().set_direction(pOrd->direction());
			legOrdPlacer->InputOrder().set_limitprice(pOrd->limitprice());
		}
	}
	
    m_triggingTimestamp = trigQuoteTimestamp;

    GoStart(openOrderId);
}

CLegOrderPlacer* CPortfolioArbitrageOrderPlacer::GetLegOrderPlacer(const string& symbol)
{
	for(vector<LegOrderPlacerPtr>::iterator iter = m_legPlacers.begin(); iter != m_legPlacers.end(); ++iter)
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
	return m_openedPosition;
}

void CPortfolioArbitrageOrderPlacer::OnPortfolioDone(PortfolioFinishState portfState)
{
	if (portfState == PortfolioFilled)
	{
		if (m_openingPosition)	// opening position done
		{
			m_openedPosition = true;
		}
		else // closing position done
		{
			m_openedPosition = false;
		}
	}
	else if (portfState == PortfolioError)
	{
		if (m_pPortf != NULL)
			m_pPortf->StopStrategy();
	}
}

CLegOrderPlacer* CPortfolioArbitrageOrderPlacer::CreateLegOrderPlacer( int openTimeout, int maxRetryTimes )
{
	return new CArbitrageLegOrderPlacer(this, openTimeout, maxRetryTimes);
}




