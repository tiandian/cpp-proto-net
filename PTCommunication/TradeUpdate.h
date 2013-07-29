#pragma once

#include "Enums.h"
#include "entity/trade.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class TradeUpdate
{
public:
	TradeUpdate(const trade::Trade* pEntity);

	///经纪公司代码
	property String ^ BrokerID;
	///投资者代码
	property String ^ InvestorID;
	///合约代码
	property String ^ InstrumentID;
	///报单引用
	property String ^ OrderRef;
	///用户代码
	property String ^ UserID;
	///交易所代码
	property String ^ ExchangeID;
	///成交编号
	property String ^ TradeID;
	///买卖方向
	property TradeDirectionType	Direction;
	///报单编号
	property String ^ OrderSysID;
	///会员代码
	property String ^ ParticipantID;
	///客户代码
	property String ^ ClientID;
	///交易角色
	property TradingRoleType	TradingRole;
	///合约在交易所的代码
	property String ^ ExchangeInstID;
	///开平标志
	property OffsetFlagType	OffsetFlag;
	///投机套保标志
	property HedgeFlagType	HedgeFlag;
	///价格
	property double	Price;
	///数量
	property int	Volume;
	///成交时期
	property String ^ TradeDate;
	///成交时间
	property String ^ TradeTime;
	///成交类型
	property TradeTypeType	TradeType;
	///成交价来源
	property PriceSourceType	PriceSource;
	///交易所交易员代码
	property String ^ TraderID;
	///本地报单编号
	property String ^ OrderLocalID;
	///结算会员编号
	property String ^ ClearingPartID;
	///业务单元
	property String ^ BusinessUnit;
	///序号
	property int	SequenceNo;
	///交易日
	property String ^ TradingDay;
	///结算编号
	property int	SettlementID;
	///经纪公司报单编号
	property int	BrokerOrderSeq;
};

}

