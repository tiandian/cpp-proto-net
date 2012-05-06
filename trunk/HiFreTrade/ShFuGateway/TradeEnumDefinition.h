#pragma once

///TFtdcOrderPriceTypeType是一个报单价格条件类型
enum OrderPriceTypeType
{
	///任意价
	ANY_PRICE = 49,
	///限价
	LIMIT_PRICE = 50,
	///最优价
	BEST_PRICE = 51,
	///最新价
	LAST_PRICE = 52,
	///最新价浮动上浮1个ticks
	LAST_PRICE_PLUS_ONE_TICKS = 53,
	///最新价浮动上浮2个ticks
	LAST_PRICE_PLUS_TWO_TICKS = 54,
	///最新价浮动上浮3个ticks
	LAST_PRICE_PLUS_THREE_TICKS = 55,
	///卖一价
	ASK_PRICE1 = 56,
	///卖一价浮动上浮1个ticks
	ASK_PRICE1_PLUS_ONE_TICKS = 57,
	///卖一价浮动上浮2个ticks
	ASK_PRICE1_PLUS_TWO_TICKS = 65,
	///卖一价浮动上浮3个ticks
	ASK_PRICE1_PLUS_THREE_TICKS = 66,
	///买一价
	BID_PRICE1 = 67,
	///买一价浮动上浮1个ticks
	BID_PRICE1_PLUS_ONE_TICKS = 68,
	///买一价浮动上浮2个ticks
	BID_PRICE1_PLUS_TWO_TICKS = 69,
	///买一价浮动上浮3个ticks
	BID_PRICE1_PLUS_THREE_TICKS = 70
};

///TFtdcDirectionType是一个买卖方向类型
enum TradeDirectionType
{
	///买
	BUY = 48,
	///卖
	SELL = 49
};

///TFtdcTimeConditionType是一个有效期类型类型
enum TimeConditionType
{
	///立即完成，否则撤销
	TC_IOC = 49,
	///本节有效
	TC_GFS = 50,
	///当日有效
	TC_GFD = 51,
	///指定日期前有效
	TC_GTD = 52,
	///撤销前有效
	TC_GTC = 53,
	///集合竞价有效
	TC_GFA = 54
};

///TFtdcVolumeConditionType是一个成交量类型类型
enum VolumeConditionType
{
	///任何数量
	VC_AV = 49,
	///最小数量
	VC_MV = 50,
	///全部数量
	VC_CV = 51
};

///TFtdcContingentConditionType是一个触发条件类型
enum ContingentConditionType
{
	///立即
	IMMEDIATELY = 49,
	///止损
	TOUCH = 50,
	///止赢
	TOUCH_PROFIT = 51,
	///预埋单
	PARKED_ORDER = 52,
	///最新价大于条件价
	LAST_PRICE_GREATER_THAN_STOP_PRICE = 53,
	///最新价大于等于条件价
	LAST_PRICE_GREATER_EQUAL_STOP_PRICE = 54,
	///最新价小于条件价
	LAST_PRICE_LESSER_THAN_STOP_PRICE = 55,
	///最新价小于等于条件价
	LAST_PRICE_LESSER_EQUAL_STOP_PRICE = 56,
	///卖一价大于条件价
	ASK_PRICE_GREATER_THAN_STOP_PRICE = 57,
	///卖一价大于等于条件价
	ASK_PRICE_GREATER_EQUAL_STOP_PRICE = 65,
	///卖一价小于条件价
	ASK_PRICE_LESSER_THAN_STOP_PRICE = 66,
	///卖一价小于等于条件价
	ASK_PRICE_LESSER_EQUAL_STOP_PRICE = 67,
	///买一价大于条件价
	BID_PRICE_GREATER_THAN_STOP_PRICE = 68,
	///买一价大于等于条件价
	BID_PRICE_GREATER_EQUAL_STOP_PRICE = 69,
	///买一价小于条件价
	BID_PRICE_LESSER_THAN_STOP_PRICE = 70,
	///买一价小于等于条件价
	BID_PRICE_LESSER_EQUAL_STOP_PRICE = 71
};

///TFtdcForceCloseReasonType是一个强平原因类型
enum ForceCloseReasonType
{
	///非强平
	NOT_FORCE_CLOSE = 48,
	///资金不足
	LACK_DEPOSIT = 49,
	///客户超仓
	CLIENT_OVER_POSITION_LIMIT = 50,
	///会员超仓
	MEMBER_OVER_POSITION_LIMIT = 51,
	///持仓非整数倍
	NOT_MULTIPLE = 52,
	///违规
	VIOLATION = 53,
	///其它
	FCC_OTHER = 54,
	///自然人临近交割
	PERSON_DELIV = 55
};

///TFtdcOrderSubmitStatusType是一个报单提交状态类型
enum OrderSubmitStatusType
{
	///已经提交
	INSERT_SUBMITTED = 48,
	///撤单已经提交
	CANCEL_SUBMITTED = 49,
	///修改已经提交
	MODIFY_SUBMITTED = 50,
	///已经接受
	ACCEPTED = 51,
	///报单已经被拒绝
	INSERT_REJECTED = 52,
	///撤单已经被拒绝
	CANCEL_REJECTED = 53,
	///改单已经被拒绝
	MODIFY_REJECTED = 54
};

///TFtdcOrderSourceType是一个报单来源类型
enum OrderSourceType
{
	///来自参与者
	PARTICIPANT = 48,
	///来自管理员
	ADMINISTRATOR = 49
};

///TFtdcOrderStatusType是一个报单状态类型
enum OrderStatusType
{
	///全部成交
	ALL_TRADED = 48,
	///部分成交还在队列中
	PART_TRADED_QUEUEING = 49,
	///部分成交不在队列中
	PART_TRADED_NOT_QUEUEING = 50,
	///未成交还在队列中
	NO_TRADE_QUEUEING = 51,
	///未成交不在队列中
	NO_TRADE_NOT_QUEUEING = 52,
	///撤单
	ORDER_CANCELED = 53,
	///未知
	STATUS_UNKNOWN = 97,
	///尚未触发
	ORDER_NOT_TOUCHED = 98,
	///已触发
	ORDER_TOUCHED = 99
};

///TFtdcOrderTypeType是一个报单类型类型
enum OrderTypeType
{
	///正常
	NORMAL_ORDER = 48,
	///报价衍生
	DERIVE_FROM_QUOTE = 49,
	///组合衍生
	DERIVE_FROM_COMBINATION = 50,
	///组合报单
	COMBINATION = 51,
	///条件单
	CONDITIONAL_ORDER = 52,
	///互换单
	SWAP_ORDER = 53
};

///TFtdcTradingRoleType是一个交易角色类型
enum TradingRoleType
{
	///代理
	ER_BROKER = 49,
	///自营
	ER_HOST = 50,
	///做市商
	ER_MAKER = 51
};

///TFtdcOffsetFlagType是一个开平标志类型
enum OffsetFlagType
{
	///开仓
	OF_OPEN = 48,
	///平仓
	OF_CLOSE = 49,
	///强平
	OF_FORCE_CLOSE = 50,
	///平今
	OF_CLOSE_TODAY = 51,
	///平昨
	OF_CLOSE_YESTERDAY = 52,
	///强减
	OF_FORCE_OFF = 53,
	///本地强平
	OF_LOCAL_FORCE_CLOSE = 54
};

///TFtdcTradeTypeType是一个成交类型类型
enum TradeTypeType
{
	///普通成交
	TRDT_COMMON = 48,
	///期权执行
	TRDT_OPTIONS_EXECUTION = 49,
	///OTC成交
	TRDT_OTC = 50,
	///期转现衍生成交
	TRDT_EFP_DERIVED = 51,
	///组合衍生成交
	TRDT_COMBINATION_DERIVED = 52
};

///TFtdcPriceSourceType是一个成交价来源类型
enum PriceSourceType
{
	///前成交价
	PSRC_LAST_PRICE = 48,
	///买委托价
	PSRC_BUY = 49,
	///卖委托价
	PSRC_SELL = 50
};

///TFtdcActionFlagType是一个操作标志类型
enum ActionFlagType
{
	///删除
	AF_Delete = 48,
	///修改
	AF_Modify = 51
};

enum HedgeFlagType {
	SPECULATION = 49,
	ARBITRAGE = 50,
	HEDGE = 51
};
