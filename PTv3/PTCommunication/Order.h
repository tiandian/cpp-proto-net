#pragma once

#include "Enums.h"
#include "entity/trade.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class Order
{
public:
	Order(const trade::Order* pEntityOrd);

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
	///报单价格条件
	property OrderPriceTypeType	OrderPriceType;
	///买卖方向
	property TradeDirectionType	Direction;
	///组合开平标志
	property String ^ CombOffsetFlag;
	///组合投机套保标志
	property String ^ CombHedgeFlag;
	///价格
	property double	LimitPrice;
	///数量
	property int	VolumeTotalOriginal;
	///有效期类型
	property TimeConditionType	TimeCondition;
	///GTD日期
	property String ^ GTDDate;
	///成交量类型
	property VolumeConditionType	VolumeCondition;
	///最小成交量
	property int	MinVolume;
	///触发条件
	property ContingentConditionType	ContingentCondition;
	///止损价
	property double	StopPrice;
	///强平原因
	property ForceCloseReasonType	ForceCloseReason;
	///自动挂起标志
	property bool	IsAutoSuspend;
	///业务单元
	property String ^ BusinessUnit;
	///请求编号
	property int	RequestID;
	// Above fields are same as InputOrder's
	////////////////////////////////////////////////////////////////////

	///本地报单编号
	property String ^ OrderLocalID;
	///交易所代码
	property String ^ ExchangeID;
	///会员代码
	property String ^ ParticipantID;
	///客户代码
	property String ^ ClientID;
	///合约在交易所的代码
	property String ^ ExchangeInstID;
	///交易所交易员代码
	property String ^ TraderID;
	///安装编号
	property int	InstallID;
	///报单提交状态
	property OrderSubmitStatusType	OrderSubmitStatus;
	///报单提示序号
	property int	NotifySequence;
	///交易日
	property String ^ TradingDay;
	///结算编号
	property int	SettlementID;
	///报单编号
	property String ^ OrderSysID;
	///报单来源
	property OrderSourceType	OrderSource;
	///报单状态
	property OrderStatusType	OrderStatus;
	///报单类型
	property OrderTypeType	OrderType;
	///今成交数量
	property int	VolumeTraded;
	///剩余数量
	property int	VolumeTotal;
	///报单日期
	property String ^ InsertDate;
	///委托时间
	property String ^ InsertTime;
	///激活时间
	property String ^ ActiveTime;
	///挂起时间
	property String ^ SuspendTime;
	///最后修改时间
	property String ^ UpdateTime;
	///撤销时间
	property String ^ CancelTime;
	///最后修改交易所交易员代码
	property String ^ ActiveTraderID;
	///结算会员编号
	property String ^ ClearingPartID;
	///序号
	property int	SequenceNo;
	///前置编号
	property int	FrontID;
	///会话编号
	property int	SessionID;
	///用户端产品信息
	property String ^ UserProductInfo;
	///状态信息
	property String ^ StatusMsg;
	///用户强评标志
	property bool	UserForceClose;
	///操作用户代码
	property String ^ ActiveUserID;
	///经纪公司报单编号
	property int	BrokerOrderSeq;
	///相关报单
	property String ^ RelativeOrderSysID;

	///优先成交
	property bool Preferred;
};

}

