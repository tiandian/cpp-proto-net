#include "StdAfx.h"
#include "RtnOrderWrapper.h"
#include "charsetconvert.h"


CRtnOrderWrapper::CRtnOrderWrapper(CThostFtdcOrderField* pOrder)
{
	memcpy(&m_orderField, pOrder, sizeof(m_orderField));
	m_orderTimestamp = boost::chrono::steady_clock::now();
}


CRtnOrderWrapper::~CRtnOrderWrapper(void)
{
}

void CRtnOrderWrapper::ToEntity( trade::Order* pOrd )
{
	///经纪公司代码
	pOrd->set_brokerid(m_orderField.BrokerID);
	///投资者代码
	pOrd->set_investorid(m_orderField.InvestorID);
	///合约代码
	pOrd->set_instrumentid(m_orderField.InstrumentID);
	///报单引用
	pOrd->set_orderref(m_orderField.OrderRef);
	///用户代码
	pOrd->set_userid(m_orderField.UserID);
	///报单价格条件
	pOrd->set_orderpricetype(static_cast	<trade::OrderPriceTypeType>(m_orderField.OrderPriceType));
	///买卖方向
	pOrd->set_direction(static_cast<trade::TradeDirectionType>(m_orderField.Direction));
	///组合开平标志
	pOrd->set_comboffsetflag(m_orderField.CombOffsetFlag);
	///组合投机套保标志
	pOrd->set_combhedgeflag(m_orderField.CombHedgeFlag);
	///价格
	pOrd->set_limitprice(m_orderField.LimitPrice);
	///数量
	pOrd->set_volumetotaloriginal(m_orderField.VolumeTotalOriginal);
	///有效期类型
	pOrd->set_timecondition(static_cast<trade::TimeConditionType>(m_orderField.TimeCondition));
	///GTD日期
	pOrd->set_gtddate(m_orderField.GTDDate);
	///成交量类型
	pOrd->set_volumecondition(static_cast<trade::VolumeConditionType>(m_orderField.VolumeCondition));
	///最小成交量
	pOrd->set_minvolume(m_orderField.MinVolume);
	///触发条件
	pOrd->set_contingentcondition(static_cast<trade::ContingentConditionType>(m_orderField.ContingentCondition));
	///止损价
	pOrd->set_stopprice(m_orderField.StopPrice);
	///强平原因
	pOrd->set_forceclosereason(static_cast<trade::ForceCloseReasonType>(m_orderField.ForceCloseReason));
	///自动挂起标志
	pOrd->set_isautosuspend(m_orderField.IsAutoSuspend != 0);
	///业务单元
	pOrd->set_businessunit(m_orderField.BusinessUnit);
	///请求编号
	pOrd->set_requestid(m_orderField.RequestID);
	// Above fields are same as InputOrder's
	////////////////////////////////////////////////////////////////////

	///本地报单编号
	pOrd->set_orderlocalid(m_orderField.OrderLocalID);
	///交易所代码
	pOrd->set_exchangeid(m_orderField.ExchangeID);
	///会员代码
	pOrd->set_participantid(m_orderField.ParticipantID);
	///客户代码
	pOrd->set_clientid(m_orderField.ClientID);
	///合约在交易所的代码
	pOrd->set_exchangeinstid(m_orderField.ExchangeInstID);
	///交易所交易员代码
	pOrd->set_traderid(m_orderField.TraderID);
	///安装编号
	pOrd->set_installid(m_orderField.InstallID);
	///报单提交状态
	pOrd->set_ordersubmitstatus(static_cast<trade::OrderSubmitStatusType>(m_orderField.OrderSubmitStatus));
	///报单提示序号
	pOrd->set_notifysequence(m_orderField.NotifySequence);
	///交易日
	pOrd->set_tradingday(m_orderField.TradingDay);
	///结算编号
	pOrd->set_settlementid(m_orderField.SettlementID);
	///报单编号
	pOrd->set_ordersysid(m_orderField.OrderSysID);
	///报单来源
	// sometimes OrderSource could be 0 insteade of '0'
	if(m_orderField.OrderSource < trade::PARTICIPANT)
		pOrd->set_ordersource(trade::PARTICIPANT);
	else
		pOrd->set_ordersource(static_cast<trade::OrderSourceType>(m_orderField.OrderSource));
	///报单状态
	pOrd->set_orderstatus(static_cast<trade::OrderStatusType>(m_orderField.OrderStatus));
	///报单类型
	// Sometimes OrderType could be 0 instead of '0'
	if(m_orderField.OrderType < trade::NORMAL_ORDER)
		pOrd->set_ordertype(trade::NORMAL_ORDER);
	else
		pOrd->set_ordertype(static_cast<trade::OrderTypeType>(m_orderField.OrderType));
	///今成交数量
	pOrd->set_volumetraded(m_orderField.VolumeTraded);
	///剩余数量
	pOrd->set_volumetotal(m_orderField.VolumeTotal);
	///报单日期
	pOrd->set_insertdate(m_orderField.InsertDate);
	///委托时间
	pOrd->set_inserttime(m_orderField.InsertTime);
	///激活时间
	pOrd->set_activetime(m_orderField.ActiveTime);
	///挂起时间
	pOrd->set_suspendtime(m_orderField.SuspendTime);
	///最后修改时间
	pOrd->set_updatetime(m_orderField.UpdateTime);
	///撤销时间
	pOrd->set_canceltime(m_orderField.CancelTime);
	///最后修改交易所交易员代码
	pOrd->set_activetraderid(m_orderField.ActiveTraderID);
	///结算会员编号
	pOrd->set_clearingpartid(m_orderField.ClearingPartID);
	///序号
	pOrd->set_sequenceno(m_orderField.SequenceNo);
	///前置编号
	pOrd->set_frontid(m_orderField.FrontID);
	///会话编号
	pOrd->set_sessionid(m_orderField.SessionID);
	///用户端产品信息
	pOrd->set_userproductinfo(m_orderField.UserProductInfo);
	///状态信息
	string ordStatusMsg;
	GB2312ToUTF_8(ordStatusMsg, m_orderField.StatusMsg);
	pOrd->set_statusmsg(ordStatusMsg);
	///用户强评标志
	pOrd->set_userforceclose(m_orderField.UserForceClose != 0);
	///操作用户代码
	pOrd->set_activeuserid(m_orderField.ActiveUserID);
	///经纪公司报单编号
	pOrd->set_brokerorderseq(m_orderField.BrokerOrderSeq);
	///相关报单
	pOrd->set_relativeordersysid(m_orderField.RelativeOrderSysID);
}

trade::OffsetFlagType CRtnOrderWrapper::Offset()
{
	if (strlen(m_orderField.CombOffsetFlag) > 0)
		return static_cast<trade::OffsetFlagType>(m_orderField.CombOffsetFlag[0]);

	return trade::OF_OPEN;
}
