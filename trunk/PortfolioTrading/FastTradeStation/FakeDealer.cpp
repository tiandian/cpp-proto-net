#include "FakeDealer.h"

#include <boost/date_time.hpp>

#ifndef WIN32
#define strcpy_s strcpy
#endif

void CopyCommonField(CThostFtdcInputOrderField * pInputOrder, CThostFtdcOrderField* pRtnOrder);


CFakeDealer::CFakeDealer(void)
{
	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	m_tradingDay = boost::gregorian::to_iso_string(d);

	m_msgPump.Init(boost::bind(&CFakeDealer::DispatchMsg, this, _1));
	m_msgPump.Start();
}

CFakeDealer::~CFakeDealer(void)
{
	m_msgPump.Stop();
}

int CFakeDealer::ReqOrderInsert( CThostFtdcInputOrderField *pInputOrder, int nRequestID )
{
	FullFillOrder(pInputOrder, nRequestID);
	return 0;
}

int CFakeDealer::ReqOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID )
{
	return 0;
}

void CFakeDealer::DispatchMsg( FakeMsgPtr& fakeMsg )
{
	FakeMsgType msgType = fakeMsg->MsgType();
	if(msgType == MSG_RTN_ORDER)
	{
		CFakeRtnOrder* fakeOrder = dynamic_cast<CFakeRtnOrder*>(fakeMsg.get());
		if(fakeOrder != NULL)
		{
			m_funcRtnOrder(fakeOrder->Msg());
		}
	}
	else if(msgType == MSG_RSP_ORDER_INSERT)
	{
		CFakeRspOrderAction* fakeOrderAct = dynamic_cast<CFakeRspOrderAction*>(fakeMsg.get());
		if(fakeOrderAct != NULL)
		{
			CThostFtdcRspInfoField rspInfo;
			memset(&rspInfo, 0, sizeof(rspInfo));
			int req = 0;
			m_funcRspOrderAction(fakeOrderAct->Msg(), &rspInfo, req, true);
		}
	}
	else if(msgType == MSG_RSP_ORDER_ACTION)
	{
		CFakeRspOrderInsert* fakeOrderIns = dynamic_cast<CFakeRspOrderInsert*>(fakeMsg.get());
		if(fakeOrderIns != NULL)
		{
			CThostFtdcRspInfoField rspInfo;
			memset(&rspInfo, 0, sizeof(rspInfo));
			int req = 0;

			m_funcRspOrderInsert(fakeOrderIns->Msg(), &rspInfo, req, true);
		}
	}
}

CFakeRtnOrder* CFakeDealer::GetAcceptOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID )
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();

	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_InsertSubmitted;
	pOrdField->OrderStatus = THOST_FTDC_OST_Unknown;
	pOrdField->VolumeTraded = 0;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal;
	strcpy_s(pOrdField->StatusMsg, "报单已提交");
	
	return pFakeOrder;
}

CFakeRtnOrder* CFakeDealer::GetPendingOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId)
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();

	sprintf(pOrdField->OrderSysID, "%d", orderSysId);
	
	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_Accepted;
	pOrdField->OrderStatus = THOST_FTDC_OST_NoTradeQueueing;
	pOrdField->VolumeTraded = 0;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal;
	strcpy_s(pOrdField->StatusMsg, "未成交");

	return pFakeOrder;
}

CFakeRtnOrder* CFakeDealer::GetFilledOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId )
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();
	sprintf(pOrdField->OrderSysID, "%d", orderSysId);

	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_Accepted;
	pOrdField->OrderStatus = THOST_FTDC_OST_AllTraded;
	pOrdField->VolumeTraded = pInputOrder->VolumeTotalOriginal;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal;
	strcpy_s(pOrdField->StatusMsg, "全部成交");

	return pFakeOrder;
}

void CFakeDealer::FullFillOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID )
{
	FakeMsgPtr msgAccept(GetAcceptOrder(pInputOrder, nRequestID));
	m_msgPump.Enqueue(msgAccept);

	int orderSysId = ++m_orderNum;

	FakeMsgPtr msgPending(GetPendingOrder(pInputOrder, nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending);

	FakeMsgPtr msgPending2(GetPendingOrder(pInputOrder, nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending2);

	FakeMsgPtr msgFilled(GetFilledOrder(pInputOrder, nRequestID, orderSysId));
	m_msgPump.Enqueue(msgFilled);
}

void CFakeDealer::SetDateField( CThostFtdcOrderField* pRtnOrder )
{
	strcpy_s(pRtnOrder->InsertDate, m_tradingDay.c_str());
	strcpy_s(pRtnOrder->TradingDay, m_tradingDay.c_str());
}

CFakeRtnOrder* CFakeDealer::CreateOrderTemplate( CThostFtdcInputOrderField * pInputOrder, int nRequestID )
{
	CFakeRtnOrder* pFakeOrder = new CFakeRtnOrder(nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();
	CopyCommonField(pInputOrder, pOrdField);
	SetDateField(pOrdField);
	return pFakeOrder;
}

void CopyCommonField(CThostFtdcInputOrderField * pInputOrder, CThostFtdcOrderField* pRtnOrder)
{
	strcpy_s(pRtnOrder->BrokerID, pInputOrder->BrokerID);
	///投资者代码
	strcpy_s(pRtnOrder->InvestorID, pInputOrder->InvestorID);
	///合约代码
	strcpy_s(pRtnOrder->InstrumentID, pInputOrder->InstrumentID);
	///报单引用
	strcpy_s(pRtnOrder->OrderRef, pInputOrder->OrderRef);
	///用户代码
	strcpy_s(pRtnOrder->UserID, pInputOrder->UserID);
	///报单价格条件
	pRtnOrder->OrderPriceType = pInputOrder->OrderPriceType;
	///买卖方向
	pRtnOrder->Direction = pInputOrder->Direction;
	///组合开平标志
	strcpy_s(pRtnOrder->CombOffsetFlag, pInputOrder->CombOffsetFlag);
	///组合投机套保标志
	strcpy_s(pRtnOrder->CombHedgeFlag, pInputOrder->CombHedgeFlag);
	///价格
	pRtnOrder->LimitPrice = pInputOrder->LimitPrice;
	///数量
	pRtnOrder->VolumeTotalOriginal = pInputOrder->VolumeTotalOriginal;
	///有效期类型
	pRtnOrder->TimeCondition = pInputOrder->TimeCondition;
	///GTD日期
	strcpy_s(pRtnOrder->GTDDate, pInputOrder->GTDDate);
	///成交量类型
	pRtnOrder->VolumeCondition = pInputOrder->VolumeCondition;
	///最小成交量
	pRtnOrder->MinVolume = pInputOrder->MinVolume;
	///触发条件
	pRtnOrder->ContingentCondition = pInputOrder->ContingentCondition;
	///止损价
	pRtnOrder->StopPrice = pInputOrder->StopPrice;
	///强平原因
	pRtnOrder->ForceCloseReason = pInputOrder->ForceCloseReason;
	///自动挂起标志
	pRtnOrder->IsAutoSuspend = pInputOrder->IsAutoSuspend;
	///业务单元
	strcpy_s(pRtnOrder->BusinessUnit, pInputOrder->BusinessUnit);
	///请求编号
	pRtnOrder->RequestID = pRtnOrder->RequestID;
	///用户强评标志
	pRtnOrder->UserForceClose = pRtnOrder->UserForceClose;
	///互换单标志
	pRtnOrder->IsSwapOrder = pRtnOrder->IsSwapOrder;
}
