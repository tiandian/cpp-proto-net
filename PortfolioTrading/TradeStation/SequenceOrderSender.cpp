#include "StdAfx.h"
#include "SequenceOrderSender.h"
#include "OrderProcessor.h"
#include "globalmembers.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>


CSequenceOrderSender::CSequenceOrderSender(
	const string& portfolioId,
	const string& mlOrderId,
	InputOrderVectorPtr pInputOrdVec, 
	COrderProcessor* orderProc, int retryTimes):
m_preferOrderCompleted(false),
m_preferOrderCompletionSuccess(false),
m_portfolioId(portfolioId),
m_mlOrderId(mlOrderId),
m_inputOrderVec(pInputOrdVec),
m_orderProc(orderProc),
m_retryTimes(retryTimes),
m_sendingOrderIndex(0)
{
}


CSequenceOrderSender::~CSequenceOrderSender(void)
{
	if(m_inputOrderVec != NULL)
		delete m_inputOrderVec;
}

void CSequenceOrderSender::Start()
{
	if(m_sendingOrderIndex == 0)
		SendOrder(m_sendingOrderIndex);
}

void CSequenceOrderSender::SendOrder(int ordIdx)
{
	const boost::shared_ptr<trade::InputOrder>& iOrd = m_inputOrderVec->at(ordIdx);
	logger.Trace(boost::str(boost::format("Sequence Sender start sending No.%d order(%s)") 
		% ordIdx % iOrd->instrumentid().c_str()));
	
	// The last order has ever modified (i.e. cancelled and resent)
	if(m_workingResubmitter.get() != NULL && m_workingResubmitter->EverModifyOrder())
	{
		// Need to re generate order ref
		string newOrdRef;
		m_orderProc->ModifyOrderRef(m_mlOrderId, iOrd->orderref(), &newOrdRef);
		iOrd->set_orderref(newOrdRef);
	}

	m_workingResubmitter = OrderResubmitterPtr(
		new COrderResubmitter(m_mlOrderId, iOrd.get(), m_orderProc, m_retryTimes));

	m_orderProc->ChangePortfolioResubmitter(m_portfolioId, m_workingResubmitter.get(), true);

	m_workingResubmitter->Start();
}

bool CSequenceOrderSender::CheckOrderStatus(trade::Order* pOrder)
{
	m_workingResubmitter->OnOrderReturn(pOrder);
	if(m_workingResubmitter->IsDone() == Filled)
	{
		logger.Trace(boost::str(boost::format("Resubmitter for %s is done")
			% pOrder->instrumentid().c_str()));
		m_orderProc->ChangePortfolioResubmitter(m_portfolioId, m_workingResubmitter.get(), false);

		++m_sendingOrderIndex;
		if(m_sendingOrderIndex < m_inputOrderVec->size())
		{
			SendOrder(m_sendingOrderIndex);
		}
		return false;
	}
	else if(m_workingResubmitter->IsDone() == Canceled)
	{
		logger.Trace(boost::str(boost::format("Resubmitter for %s is finally canceled.(out of retry)")
			% pOrder->instrumentid().c_str()));
		m_orderProc->ChangePortfolioResubmitter(m_portfolioId, m_workingResubmitter.get(), false);
		return true;
	}
	else // Resubmitter is not done
	{
		return false;
	}
}

void CSequenceOrderSender::OrderPending()
{
	logger.Trace(boost::str(boost::format("Order(%s) is pending")
		% m_workingResubmitter->Symbol().c_str()));
	m_workingResubmitter->OrderPending();
}

void CSequenceOrderSender::OrderInsertFailed()
{
	m_orderProc->ChangePortfolioResubmitter(m_portfolioId, m_workingResubmitter.get(), false);
}
