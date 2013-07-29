#include "StdAfx.h"
#include "Order.h"
#include "charsetconvert.h"

PTEntity::Order::Order( const trade::Order* pEntityOrd )
{
	BrokerID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->brokerid().c_str());
	InvestorID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->investorid().c_str());
	InstrumentID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->instrumentid().c_str());
	OrderRef = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->orderref().c_str());
	UserID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->userid().c_str());
	OrderPriceType = static_cast<OrderPriceTypeType>(pEntityOrd->orderpricetype());
	Direction = static_cast<TradeDirectionType>(pEntityOrd->direction());
	CombOffsetFlag = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->comboffsetflag().c_str());
	LimitPrice = pEntityOrd->limitprice();
	VolumeTotalOriginal = pEntityOrd->volumetotaloriginal();
	TimeCondition = static_cast<TimeConditionType>(pEntityOrd->timecondition());
	GTDDate = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->gtddate().c_str());
	VolumeCondition = static_cast<VolumeConditionType>(pEntityOrd->volumecondition());
	MinVolume = pEntityOrd->minvolume();
	ContingentCondition = static_cast<ContingentConditionType>(pEntityOrd->contingentcondition());
	StopPrice = pEntityOrd->stopprice();
	ForceCloseReason = static_cast<ForceCloseReasonType>(pEntityOrd->forceclosereason());
	IsAutoSuspend = pEntityOrd->isautosuspend();
	BusinessUnit = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->businessunit().c_str());
	RequestID = pEntityOrd->requestid();
	
	OrderLocalID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->orderlocalid().c_str());
	ExchangeID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->exchangeid().c_str());
	ParticipantID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->participantid().c_str());;
	ClientID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->clientid().c_str());
	ExchangeInstID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->exchangeinstid().c_str());
	TraderID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->traderid().c_str());
	InstallID = pEntityOrd->installid();
	OrderSubmitStatus = static_cast<OrderSubmitStatusType>(pEntityOrd->ordersubmitstatus());
	NotifySequence = pEntityOrd->notifysequence();
	TradingDay = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->tradingday().c_str());
	SettlementID = pEntityOrd->settlementid();
	OrderSysID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->ordersysid().c_str());
	OrderSource = static_cast<OrderSourceType>(pEntityOrd->ordersource());
	OrderStatus = static_cast<OrderStatusType>(pEntityOrd->orderstatus());
	OrderType = static_cast<OrderTypeType>(pEntityOrd->ordertype());
	VolumeTraded = pEntityOrd->volumetraded();
	VolumeTotal = pEntityOrd->volumetotal();
	InsertDate = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->insertdate().c_str());
	InsertTime = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->inserttime().c_str());
	ActiveTime = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->activetime().c_str());
	SuspendTime = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->suspendtime().c_str());
	UpdateTime = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->updatetime().c_str());
	CancelTime = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->canceltime().c_str());
	ActiveTraderID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->activetraderid().c_str());
	ClearingPartID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->clearingpartid().c_str());
	SequenceNo = pEntityOrd->sequenceno();
	FrontID = pEntityOrd->frontid();
	SessionID = pEntityOrd->sessionid();
	UserProductInfo = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->userproductinfo().c_str());

	wchar_t* uniStr = UTF8ToUnicode(pEntityOrd->statusmsg().c_str());
	StatusMsg = Marshal::PtrToStringAuto((IntPtr)uniStr);
	delete[] uniStr;
	
	UserForceClose = pEntityOrd->userforceclose();
	ActiveUserID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->activeuserid().c_str());
	BrokerOrderSeq = pEntityOrd->brokerorderseq();
	RelativeOrderSysID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntityOrd->relativeordersysid().c_str());
	Preferred = pEntityOrd->preferred();
}
