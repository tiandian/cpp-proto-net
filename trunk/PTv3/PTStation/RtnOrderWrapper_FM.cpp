#include "StdAfx.h"
#include "RtnOrderWrapper_FM.h"
#include "charsetconvert.h"


CRtnOrderWrapper::CRtnOrderWrapper(CUstpFtdcOrderField* pOrder)
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
	pOrd->set_orderref(m_orderField.UserOrderLocalID);
	///用户代码
	pOrd->set_userid(m_orderField.UserID);
	///报单价格条件
	pOrd->set_orderpricetype(static_cast	<trade::OrderPriceTypeType>(m_orderField.OrderPriceType));
	///买卖方向
	pOrd->set_direction(static_cast<trade::TradeDirectionType>(m_orderField.Direction));
	string a;
	
	char tmp[2] = { 0, 0 };
	///开平标志
	tmp[0] = m_orderField.OffsetFlag;
	pOrd->set_comboffsetflag(tmp);
	///投机套保标志
	tmp[0] = m_orderField.HedgeFlag;
	pOrd->set_combhedgeflag(tmp);
	///价格
	pOrd->set_limitprice(m_orderField.LimitPrice);
	///数量
	pOrd->set_volumetotaloriginal(m_orderField.Volume);
	///有效期类型
	pOrd->set_timecondition(static_cast<trade::TimeConditionType>(m_orderField.TimeCondition));
	///GTD日期
	pOrd->set_gtddate(m_orderField.GTDDate);
	///成交量类型
	pOrd->set_volumecondition(static_cast<trade::VolumeConditionType>(m_orderField.VolumeCondition));
	///最小成交量
	pOrd->set_minvolume(m_orderField.MinVolume);
	///触发条件
	pOrd->set_contingentcondition(trade::IMMEDIATELY);
	///止损价
	pOrd->set_stopprice(m_orderField.StopPrice);
	///强平原因
	pOrd->set_forceclosereason(static_cast<trade::ForceCloseReasonType>(m_orderField.ForceCloseReason));
	///自动挂起标志
	pOrd->set_isautosuspend(m_orderField.IsAutoSuspend != 0);
	///业务单元
	pOrd->set_businessunit(m_orderField.BusinessUnit);
	///请求编号
	pOrd->set_requestid(0);
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
	pOrd->set_exchangeinstid("");
	///交易所交易员代码
	pOrd->set_traderid("");
	///安装编号
	pOrd->set_installid(0);
	///报单提交状态
	pOrd->set_ordersubmitstatus(OrderSubmitStatus());
	///报单提示序号
	pOrd->set_notifysequence(0);
	///交易日
	pOrd->set_tradingday(m_orderField.TradingDay);
	///结算编号
	pOrd->set_settlementid(0);
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
	pOrd->set_ordertype(trade::NORMAL_ORDER);
	///今成交数量
	pOrd->set_volumetraded(m_orderField.VolumeTraded);
	///剩余数量
	pOrd->set_volumetotal(m_orderField.VolumeRemain);
	///报单日期
	pOrd->set_insertdate("");
	///委托时间
	pOrd->set_inserttime(m_orderField.InsertTime);
	///激活时间
	pOrd->set_activetime("");
	///挂起时间
	pOrd->set_suspendtime("");
	///最后修改时间
	pOrd->set_updatetime("");
	///撤销时间
	pOrd->set_canceltime(m_orderField.CancelTime);
	///最后修改交易所交易员代码
	pOrd->set_activetraderid("");
	///结算会员编号
	pOrd->set_clearingpartid("");
	///序号
	pOrd->set_sequenceno(0);
	///前置编号
	pOrd->set_frontid(0);
	///会话编号
	pOrd->set_sessionid(0);
	///用户端产品信息
	pOrd->set_userproductinfo("");
	///状态信息
	pOrd->set_statusmsg("");
	///用户强评标志
	pOrd->set_userforceclose(false);
	///操作用户代码
	pOrd->set_activeuserid("");
	///经纪公司报单编号
	pOrd->set_brokerorderseq(0);
	///相关报单
	pOrd->set_relativeordersysid("");
}
