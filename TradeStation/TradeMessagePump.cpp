#include "StdAfx.h"
#include "TradeMessagePump.h"
#include "globalmembers.h"
#include "TradeAgentCallback.h"


CTradeMessagePump::CTradeMessagePump(void):
m_bufferRunner(boost::bind(&CTradeMessagePump::DispatchMessage, this, _1))
{
}


CTradeMessagePump::~CTradeMessagePump(void)
{
	m_bufferRunner.Stop();
}

void CTradeMessagePump::DispatchMessage( MessagePacketPtr& message )
{
	TD_MSG_TYPE msgTy = message->MsgType();
	switch(msgTy)
	{
	case RTN_ORDER:
		{
			trade::Order* pOrder = static_cast<trade::Order*>(message->Data());
			m_orderProcessor->OnRtnOrder(pOrder);
			delete pOrder;
		}
		break;
	case RSP_LOGIN:
		{
			CRspUserLogin* rspUserLogin = static_cast<CRspUserLogin*>(message->Data());
			m_orderProcessor->OnRspUserLogin(rspUserLogin->LoginSuccess, rspUserLogin->ErrorMsg, rspUserLogin->InitOrderRef);
			delete rspUserLogin;
		}
		break;
	case RSP_ACCOUNT:
		{
			trade::AccountInfo* pAcct = static_cast<trade::AccountInfo*>(message->Data());
			m_orderProcessor->OnRspQryTradingAccount(*pAcct);
			delete pAcct;
		}
		break;
	case RSP_ORDER_INSERT:
		{
			CRspOrderInsert* pOrdInsert = static_cast<CRspOrderInsert*>(message->Data());
			m_orderProcessor->OnRspOrderInsert(pOrdInsert->InsertSucess, pOrdInsert->OrderRef, pOrdInsert->ErrorMsg);
			delete pOrdInsert;
		}
		break;
	case RTN_TRADE:
		{
			trade::Trade* pTrade = static_cast<trade::Trade*>(message->Data());
			m_orderProcessor->OnRtnTrade(pTrade);
			delete pTrade;
		}
		break;
	case RSP_POSITION:
		{
			trade::PositionDetailInfo* pPosition = static_cast<trade::PositionDetailInfo*>(message->Data());
			m_orderProcessor->OnRspQryInvestorPositionDetail(pPosition);
			delete pPosition;
		}
		break;
	default:
		logger.Warning("Unexpected trade message!");
	}
}

void CTradeMessagePump::Enqueue( MessagePacketPtr& msgPacket )
{
	m_bufferRunner.Enqueue(msgPacket);
}

void CTradeMessagePump::Init( CTradeAgentCallback* pOrdProcessor )
{
	m_orderProcessor = pOrdProcessor;
	m_bufferRunner.Start();
}
