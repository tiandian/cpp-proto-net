#include "StdAfx.h"
#include "OrderMsg.h"


COrderMsg::COrderMsg(void):
CMessage(ORDER)
{
	memset(&m_innerData, 0, sizeof(m_innerData));
}


COrderMsg::~COrderMsg(void)
{
}

void COrderMsg::SetData( CThostFtdcOrderField* pOrder )
{
	SafeStringCopy(m_innerData.caSymbol, pOrder->InstrumentID, SYMBOL_LEN);
	SafeStringCopy(m_innerData.caOrderRef, pOrder->OrderRef, ORDER_REF_LEN);
	SafeStringCopy(m_innerData.caInvestorID, pOrder->InvestorID, INVESTORID_LEN);
	SafeStringCopy(m_innerData.caUserID, pOrder->UserID, USERID_LEN);
	m_innerData.cHedgeFlag = pOrder->CombHedgeFlag[0];
	m_innerData.iEntryExitType = GetEntryExitType(pOrder->Direction, pOrder->CombOffsetFlag[0]);
	m_innerData.cPriceType = pOrder->OrderPriceType;
	m_innerData.dLimitPrice = pOrder->LimitPrice;
	m_innerData.iVolumeTotalOriginal = pOrder->VolumeTotalOriginal;
	SafeStringCopy(m_innerData.caExchangeID, pOrder->ExchangeID, EXCHANGEID_LEN);
	m_innerData.iOrderStatus = GetOrderStatus(pOrder->OrderSubmitStatus, pOrder->OrderStatus);
	SafeStringCopy(m_innerData.caOrderSysID, pOrder->OrderSysID, ORDER_SYS_ID_LEN);
	m_innerData.iVolumeTraded = pOrder->VolumeTraded;
	m_innerData.iVolumeRemain = pOrder->VolumeTotal;
	SafeStringCopy(m_innerData.caInsertDate, pOrder->InsertDate, TRADINDG_DAY_LEN);
	SafeStringCopy(m_innerData.caInsertTime, pOrder->InsertTime, UPDATE_TIME_LEN);
	SafeStringCopy(m_innerData.caUpdateTime, pOrder->UpdateTime, UPDATE_TIME_LEN);
	SafeStringCopy(m_innerData.caCancelTime, pOrder->CancelTime, UPDATE_TIME_LEN);
	m_innerData.iFrontID = pOrder->FrontID;
	m_innerData.iSessionID = pOrder->SessionID;
}

int COrderMsg::GetEntryExitType( char direction, char offsetFlag )
{
	if(offsetFlag == THOST_FTDC_OF_Open)
	{
		return direction == THOST_FTDC_D_Buy ? LONG_OPEN : SHORT_OPEN;
	}
	else
	{
		return direction == THOST_FTDC_D_Sell ? LONG_CLOSE : SHORT_CLOSE;
	}
}

int COrderMsg::GetOrderStatus( char submitStatus, char orderStatus )
{
	if(submitStatus == THOST_FTDC_OSS_InsertSubmitted)
	{
		if(orderStatus == THOST_FTDC_OST_AllTraded)
			return FULL_FILLED;
		else if(orderStatus == THOST_FTDC_OST_NoTradeQueueing || orderStatus == THOST_FTDC_OST_NoTradeNotQueueing)
			return PENDING;
		else if(orderStatus == THOST_FTDC_OST_PartTradedQueueing || orderStatus == THOST_FTDC_OST_PartTradedNotQueueing)
			return PARTIALLY_FILLED;
		else
			return ORDER_SUBMIT;
	}
	if(submitStatus == THOST_FTDC_OSS_CancelSubmitted)
	{
		if(orderStatus == THOST_FTDC_OST_Canceled)
			return CANCELED;
		else
			return UNKNOWN;
	}
	if(submitStatus == THOST_FTDC_OSS_Accepted)
	{
		if(orderStatus == THOST_FTDC_OST_AllTraded)
			return FULL_FILLED;
		else if(orderStatus == THOST_FTDC_OST_NoTradeQueueing || orderStatus == THOST_FTDC_OST_NoTradeNotQueueing)
			return PENDING;
		else if(orderStatus == THOST_FTDC_OST_PartTradedQueueing || orderStatus == THOST_FTDC_OST_PartTradedNotQueueing)
			return PARTIALLY_FILLED;
		else if(orderStatus == THOST_FTDC_OST_Canceled)
			return CANCELED;
		else
			return UNKNOWN;
	}
	else if(submitStatus >= THOST_FTDC_OSS_InsertRejected)
	{
		return REJECTED;
	}
	else
	{
		return UNKNOWN;
	}
}
