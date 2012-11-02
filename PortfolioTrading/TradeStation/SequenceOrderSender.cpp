#include "StdAfx.h"
#include "SequenceOrderSender.h"

#include <boost/foreach.hpp>


CSequenceOrderSender::CSequenceOrderSender(
	const string& portfolioId,
	const string& mlOrderId,
	InputOrderVectorPtr pInputOrdVec, 
	SubmitOrderFunc submitOrdFunc):
m_preferOrderCompleted(false),
m_preferOrderCompletionSuccess(false),
m_portfolioId(portfolioId),
m_mlOrderId(mlOrderId),
m_inputOrderVec(pInputOrdVec),
m_submitOrderFunc(submitOrdFunc)
{
}


CSequenceOrderSender::~CSequenceOrderSender(void)
{
	if(m_inputOrderVec != NULL)
		delete m_inputOrderVec;
}

void CSequenceOrderSender::Start()
{
	m_th = boost::thread(boost::bind(&CSequenceOrderSender::SendingProc, this));
}

void CSequenceOrderSender::SendingProc()
{
	BOOST_FOREACH(const boost::shared_ptr<trade::InputOrder>& iOrd, *m_inputOrderVec)
	{
		if(!m_submitOrderFunc.empty())
			m_submitOrderFunc(m_portfolioId, iOrd.get(), m_mlOrderId);

		if(!m_preferOrderCompleted)
		{
			boost::mutex::scoped_lock lock(m_mut);
			m_preferDoneCond.wait(lock);
			if(m_preferOrderCompletionSuccess)
				m_preferOrderCompleted = true;
			else // if prefer order canceled or submit failed, do NOT continue any more
				break;	
		}
	}
}

void CSequenceOrderSender::OrderDone(bool success)
{
	boost::mutex::scoped_lock l(m_mut);
	m_preferOrderCompletionSuccess = success;
	m_preferDoneCond.notify_one();
}
