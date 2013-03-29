#include "ScalperPortfolioOrderPlacer.h"
#include "AsyncScalperEventFirer.h"
#include "OrderProcessor2.h"
#include "SgOrderStateMachine.h"
#include "OrderState.h"

void CScalperPortfolioOrderPlacer::Send()
{
	m_pOrderProcessor->PublishMultiLegOrderUpdate(m_mlOrder.get());

	assert(m_sgOrderPlacers.size() > 0);
	//This should be open order
	CSgOrderPlacer* pPlacer = m_sgOrderPlacers.at(0);
	const string& ordRef = pPlacer->Id();
	m_openOrderTimer = boost::shared_ptr<CAsyncOpenOrderTimer>(
		new CAsyncOpenOrderTimer(m_pOrderProcessor, ordRef, 200));
	pPlacer->Do();
	m_openOrderTimer->Run();
}

bool CScalperPortfolioOrderPlacer::SendNext(op2::COrderEvent* transEvent)
{
	//The next one should be close order
	CSgOrderPlacer* pPlacer = m_sgOrderPlacers.at(1);
	const string& ordRef = pPlacer->Id();
	long quoteTimestamp = pPlacer->QuoteTimestamp();
	const string& symbol = pPlacer->Symbol();
	CLeg* pLeg = m_pPortf->GetLeg(symbol);

	// Partically for Scalper strategy
	// ensure close order qty align to opened quantity
	if(m_pPortf->SelfClose())
	{
		LegCompletedEvent* pLegCompleteEvent = dynamic_cast<LegCompletedEvent*>(transEvent);
		if(pLegCompleteEvent != NULL)
		{
			int finished = pLegCompleteEvent->Finished();
			pPlacer->AdjustQuantity(finished);
		}
	}

	m_nextQuoteWaiter = boost::shared_ptr<CAsyncNextQuoteWaiter>(
		new CAsyncNextQuoteWaiter(m_pOrderProcessor, ordRef, pLeg, quoteTimestamp));
	m_nextQuoteWaiter->Run();

	pPlacer->Do();

	return true;
}
