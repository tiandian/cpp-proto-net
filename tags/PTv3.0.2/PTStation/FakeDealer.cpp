#include "StdAfx.h"
#include "FakeDealer.h"

#ifndef WIN32
#define strcpy_s strcpy
#endif

void CopyCommonField(CThostFtdcInputOrderField * pInputOrder, CThostFtdcOrderField* pRtnOrder);


CFakeDealer::CFakeDealer(void)
	: m_orderNum(1000)
	, FRONT_ID(0)
	, SESSION_ID(0)
	, m_pendingOrdSysId(0)
	, m_partiallyFilledAmount(1)
	, m_testPartiallyFill(false)
{
	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	m_tradingDay = boost::gregorian::to_iso_string(d);

	boost::posix_time::time_facet* const timeFormat = new boost::posix_time::time_facet("%H:%M:%S");
	m_timeStream.imbue(std::locale(m_timeStream.getloc(), timeFormat));

	memset(&m_pendingInputOrder, 0, sizeof(m_pendingInputOrder));

	m_msgPump.Init(boost::bind(&CFakeDealer::DispatchMsg, this, _1));
	m_msgPump.Start();
}

CFakeDealer::~CFakeDealer(void)
{
	m_msgPump.Stop();
}

int CFakeDealer::ReqOrderInsert( CThostFtdcInputOrderField *pInputOrder, int nRequestID )
{
	static int times = 0;
	++times;

	boost::shared_ptr<CThostFtdcInputOrderField> tmpInputOrder( new CThostFtdcInputOrderField);
	memcpy(tmpInputOrder.get(), pInputOrder, sizeof(CThostFtdcInputOrderField));

	if(m_testPartiallyFill)
	{
		// Partially fill test
		if(times % 4 == 1)
		{
			boost::thread thIns(boost::bind(
				//&CFakeDealer::PartiallyFillOrder, this, tmpInputOrder, nRequestID)
				&CFakeDealer::FullFillOrder, this, tmpInputOrder, nRequestID)
				);
		}
		else if(times % 4 == 2)
		{
			boost::thread thIns(boost::bind(
				//&CFakeDealer::FullFillOrder, this, tmpInputOrder, nRequestID)
				&CFakeDealer::PartiallyFillOrder, this, tmpInputOrder, nRequestID)
				);
		}
		else if(times % 4 == 3)
		{
			boost::thread thIns(boost::bind(
				//&CFakeDealer::FullFillOrder, this, tmpInputOrder, nRequestID)
				&CFakeDealer::PartiallyFillOrder, this, tmpInputOrder, nRequestID)
				);
		}
		else{
			boost::thread thIns(boost::bind(
				//&CFakeDealer::PartiallyFillOrder, this, tmpInputOrder, nRequestID)
				&CFakeDealer::FullFillOrder, this, tmpInputOrder, nRequestID)
				);
		}
	}
	else
	{
		// Fill and pending
		if(times % 2 == 1)
		{
			boost::thread thIns(boost::bind(
				&CFakeDealer::FullFillOrder, this, tmpInputOrder, nRequestID)
				//&CFakeDealer::PendingOrder, this, pInputOrder, nRequestID)
				);
		}
		else
		{
			boost::thread thIns(boost::bind(
				//&CFakeDealer::FullFillOrder, this, pInputOrder, nRequestID)
				&CFakeDealer::PendingOrder, this, tmpInputOrder, nRequestID)
				);
		}
	}
	
	return 0;
}

int CFakeDealer::ReqOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID )
{
	boost::shared_ptr<CThostFtdcInputOrderActionField> tmpInputOrderAct( new CThostFtdcInputOrderActionField);
	memcpy(tmpInputOrderAct.get(), pInputOrderAction, sizeof(CThostFtdcInputOrderActionField));

	boost::thread thIns(boost::bind(
		&CFakeDealer::CancelOrder, this, tmpInputOrderAct, nRequestID)
		);
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
	strcpy_s(pOrdField->StatusMsg, "�������ύ");
	
	return pFakeOrder;
}

CFakeRtnOrder* CFakeDealer::GetPendingOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId, int amount )
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();

	sprintf(pOrdField->OrderSysID, "%d", orderSysId);
	
	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_Accepted;
	pOrdField->OrderStatus = amount > 0 ? THOST_FTDC_OST_PartTradedQueueing : THOST_FTDC_OST_NoTradeQueueing;
	pOrdField->VolumeTraded = amount;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal - amount;

	strcpy_s(pOrdField->StatusMsg, amount > 0 ? "���ֳɽ�" : "δ�ɽ�");

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
	strcpy_s(pOrdField->StatusMsg, "ȫ���ɽ�");

	return pFakeOrder;
}

CFakeRtnOrder* CFakeDealer::GetPartiallyFilledOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId, int amount )
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();
	sprintf(pOrdField->OrderSysID, "%d", orderSysId);

	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_Accepted;
	pOrdField->OrderStatus = THOST_FTDC_OST_PartTradedQueueing;
	pOrdField->VolumeTraded = amount;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal - amount;
	strcpy_s(pOrdField->StatusMsg, "���ֳɽ�");

	return pFakeOrder;

}


CFakeRtnOrder* CFakeDealer::GetCanceledOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId, int amount )
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();
	sprintf(pOrdField->OrderSysID, "%d", orderSysId);

	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_Accepted;
	pOrdField->OrderStatus = THOST_FTDC_OST_Canceled;
	pOrdField->VolumeTraded = amount;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal - amount;
	strcpy_s(pOrdField->StatusMsg, "�ѳ���");

	return pFakeOrder;
}


void CFakeDealer::FullFillOrder( boost::shared_ptr<CThostFtdcInputOrderField> pInputOrder, int nRequestID )
{
	FakeMsgPtr msgAccept(GetAcceptOrder(pInputOrder.get(), nRequestID));
	m_msgPump.Enqueue(msgAccept);

	int orderSysId = ++m_orderNum;

	FakeMsgPtr msgPending(GetPendingOrder(pInputOrder.get(), nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending);

	FakeMsgPtr msgPending2(GetPendingOrder(pInputOrder.get(), nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending2);

	FakeMsgPtr msgFilled(GetFilledOrder(pInputOrder.get(), nRequestID, orderSysId));
	m_msgPump.Enqueue(msgFilled);
}


void CFakeDealer::PartiallyFillOrder( boost::shared_ptr<CThostFtdcInputOrderField> pInputOrder, int nRequestID )
{
	m_pendingInputOrder = *pInputOrder;

	FakeMsgPtr msgAccept(GetAcceptOrder(pInputOrder.get(), nRequestID));
	m_msgPump.Enqueue(msgAccept);

	int orderSysId = ++m_orderNum;

	FakeMsgPtr msgPending(GetPendingOrder(pInputOrder.get(), nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending);

	FakeMsgPtr msgPending2(GetPendingOrder(pInputOrder.get(), nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending2);

	FakeMsgPtr msgFilled(GetPartiallyFilledOrder(pInputOrder.get(), nRequestID, orderSysId, m_partiallyFilledAmount));
	m_msgPump.Enqueue(msgFilled);
}


void CFakeDealer::PendingOrder( boost::shared_ptr<CThostFtdcInputOrderField> pInputOrder, int nRequestID )
{
	m_pendingInputOrder = *pInputOrder;

	FakeMsgPtr msgAccept(GetAcceptOrder(pInputOrder.get(), nRequestID));
	m_msgPump.Enqueue(msgAccept);

	int orderSysId = ++m_orderNum;

	FakeMsgPtr msgPending(GetPendingOrder(pInputOrder.get(), nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending);
}

void CFakeDealer::CancelOrder( boost::shared_ptr<CThostFtdcInputOrderActionField> pInputOrderAction, int nRequestID )
{
	int orderSysId = atoi(pInputOrderAction->OrderSysID);

	int filledAmount = m_testPartiallyFill ? m_partiallyFilledAmount : 0;

	FakeMsgPtr msgPending(GetPendingOrder(&m_pendingInputOrder, nRequestID, orderSysId, filledAmount));
	m_msgPump.Enqueue(msgPending);

	FakeMsgPtr msgCanceled(GetCanceledOrder(&m_pendingInputOrder, nRequestID, orderSysId, filledAmount));
	m_msgPump.Enqueue(msgCanceled);
}

void CFakeDealer::SetDateField( CThostFtdcOrderField* pRtnOrder )
{
	strcpy_s(pRtnOrder->InsertDate, m_tradingDay.c_str());
	strcpy_s(pRtnOrder->TradingDay, m_tradingDay.c_str());

	{
		//boost::lock_guard<boost::mutex> lock(m_mutTimeFormat);
		const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		m_timeStream << now;
		string timeText = m_timeStream.str();
		strcpy_s(pRtnOrder->InsertTime, timeText.c_str());
		m_timeStream.str("");
		m_timeStream.clear();
	}
}

CFakeRtnOrder* CFakeDealer::CreateOrderTemplate( CThostFtdcInputOrderField * pInputOrder, int nRequestID )
{
	CFakeRtnOrder* pFakeOrder = new CFakeRtnOrder(nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();
	CopyCommonField(pInputOrder, pOrdField);
	SetDateField(pOrdField);

	pOrdField->FrontID = FRONT_ID;
	pOrdField->SessionID = SESSION_ID;

	return pFakeOrder;
}

void CopyCommonField(CThostFtdcInputOrderField * pInputOrder, CThostFtdcOrderField* pRtnOrder)
{
	strcpy_s(pRtnOrder->BrokerID, pInputOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy_s(pRtnOrder->InvestorID, pInputOrder->InvestorID);
	///��Լ����
	strcpy_s(pRtnOrder->InstrumentID, pInputOrder->InstrumentID);
	///��������
	strcpy_s(pRtnOrder->OrderRef, pInputOrder->OrderRef);
	///�û�����
	strcpy_s(pRtnOrder->UserID, pInputOrder->UserID);
	///�����۸�����
	pRtnOrder->OrderPriceType = pInputOrder->OrderPriceType;
	///��������
	pRtnOrder->Direction = pInputOrder->Direction;
	///��Ͽ�ƽ��־
	strcpy_s(pRtnOrder->CombOffsetFlag, pInputOrder->CombOffsetFlag);
	///���Ͷ���ױ���־
	strcpy_s(pRtnOrder->CombHedgeFlag, pInputOrder->CombHedgeFlag);
	///�۸�
	pRtnOrder->LimitPrice = pInputOrder->LimitPrice;
	///����
	pRtnOrder->VolumeTotalOriginal = pInputOrder->VolumeTotalOriginal;
	///��Ч������
	pRtnOrder->TimeCondition = pInputOrder->TimeCondition;
	///GTD����
	strcpy_s(pRtnOrder->GTDDate, pInputOrder->GTDDate);
	///�ɽ�������
	pRtnOrder->VolumeCondition = pInputOrder->VolumeCondition;
	///��С�ɽ���
	pRtnOrder->MinVolume = pInputOrder->MinVolume;
	///��������
	pRtnOrder->ContingentCondition = pInputOrder->ContingentCondition;
	///ֹ���
	pRtnOrder->StopPrice = pInputOrder->StopPrice;
	///ǿƽԭ��
	pRtnOrder->ForceCloseReason = pInputOrder->ForceCloseReason;
	///�Զ������־
	pRtnOrder->IsAutoSuspend = pInputOrder->IsAutoSuspend;
	///ҵ��Ԫ
	strcpy_s(pRtnOrder->BusinessUnit, pInputOrder->BusinessUnit);
	///������
	pRtnOrder->RequestID = pRtnOrder->RequestID;
	///�û�ǿ����־
	pRtnOrder->UserForceClose = pRtnOrder->UserForceClose;
	///��������־
	pRtnOrder->IsSwapOrder = pRtnOrder->IsSwapOrder;
}