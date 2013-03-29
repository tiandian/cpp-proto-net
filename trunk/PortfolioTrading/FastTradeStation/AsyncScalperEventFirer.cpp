#include "AsyncScalperEventFirer.h"
#include "OrderProcessor2.h"
#include "Portfolio.h"
#include "SgOrderStateMachine.h"
#include "globalmembers.h"

#include <boost/format.hpp>

void CAsyncOpenOrderTimer::FireEvent( const boost::system::error_code& e )
{
	PendingTimeUpEvent timeupEvent;
	m_pOrdProc->RaiseSGOrderPlacerEvent(m_orderRef, &timeupEvent);
}

void CAsyncNextQuoteWaiter::FireEvent(double last, double ask, double bid)
{
	NextQuoteInEvent nextQuoteEvent(last, ask, bid);
	m_pOrdProc->RaiseSGOrderPlacerEvent(m_orderRef, &nextQuoteEvent);
}

void CAsyncNextQuoteWaiter::GetUpdateQuote()
{
	bool quoteUpdated = m_pLeg->IsQuoteUpdated(m_quoteTimestamp);
	if(!quoteUpdated)
	{
		logger.Warning(boost::str(boost::format("Order(%s)'s quote didn't get updated after cancelled")
			% m_pLeg->Symbol()));
	}
	if(!m_canceled)
		FireEvent(m_pLeg->Last(), m_pLeg->Ask(), m_pLeg->Bid());
}
