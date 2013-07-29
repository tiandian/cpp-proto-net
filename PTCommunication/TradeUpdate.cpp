#include "StdAfx.h"
#include "TradeUpdate.h"


namespace PTEntity {

TradeUpdate::TradeUpdate(const trade::Trade* pEntity)
{
	BrokerID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->brokerid().c_str());
	InvestorID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->investorid().c_str());
	InstrumentID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->instrumentid().c_str());
	OrderRef = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->orderref().c_str());
	UserID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->userid().c_str());
	ExchangeID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->exchangeid().c_str());
	TradeID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->tradeid().c_str());
	Direction = static_cast<TradeDirectionType>(pEntity->direction());
	OrderSysID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->ordersysid().c_str());
	ParticipantID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->participantid().c_str());
	ClientID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->clientid().c_str());
	TradingRole = static_cast<TradingRoleType>(pEntity->tradingrole());
	ExchangeInstID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->exchangeinstid().c_str());
	OffsetFlag = static_cast<OffsetFlagType>(pEntity->offsetflag());
	HedgeFlag = static_cast<HedgeFlagType>(pEntity->hedgeflag());
	Price = pEntity->price();
	Volume = pEntity->volume();
	TradeDate = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->tradedate().c_str());
	TradeTime = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->tradetime().c_str());
	TradeType = static_cast<TradeTypeType>(pEntity->tradetype());
	PriceSource = static_cast<PriceSourceType>(pEntity->pricesource());
	TraderID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->traderid().c_str());
	OrderLocalID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->orderlocalid().c_str());
	ClearingPartID = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->clearingpartid().c_str());
	BusinessUnit = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->businessunit().c_str());
	SequenceNo = pEntity->sequenceno();
	TradingDay = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->tradingday().c_str());
	SettlementID = pEntity->settlementid();
	BrokerOrderSeq = pEntity->brokerorderseq();
}

}
