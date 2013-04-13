#include "AsyncScalperEventFirer.h"
#include "OrderProcessor2.h"
#include "Portfolio.h"
#include "globalmembers.h"

#include <boost/format.hpp>

void CAsyncOpenOrderTimer::FireEvent( const boost::system::error_code& e )
{
	if(e.value() == boost::asio::error::operation_aborted)
		return; // timer canceled

	logger.Info(boost::str(boost::format("Opening order ref(%s) Time up") % m_orderRef));
	m_pOrdPlacer->OnPendingTimeUp();
}

