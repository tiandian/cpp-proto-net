#pragma once

#include "InputOrder.h"

#include <string>

using namespace std;

class CReturnOrder : public CInputOrder
{
public:
	CReturnOrder(void);
	~CReturnOrder(void);

	///本地报单编号
	const string& orderlocalid(){return m_orderLocalId;}
	void set_orderlocalid(const string& orderLocalId){m_orderLocalId = orderLocalId;}

	///交易所代码
	const string& exchangeid(){return m_exchangeId;}
	void set_exchangeid(const string& exchId){m_exchangeId = exchId;}

	///会员代码
	const string& participantid(){return m_participantId;}
	void set_participantid(const string& partId){m_participantId = partId;}

	///客户代码
	const string& clientid(){return m_clientId;}
	void set_clientid(const string& clientId){m_clientId = clientId;}

	///合约在交易所的代码
	const string& exchangeinstid(){return m_exchangeInstId;}
	void set_exchangeinstid(const string& exchInstId){m_exchangeInstId = exchInstId;}

	///交易所交易员代码
	const string& traderid(){return m_traderId;}
	void set_traderid(const string& traderId){m_traderId = traderId;}

	///安装编号
	int installid(){return m_installId;}
	void set_installid(int installId){m_installId = installId;}

	///报单提交状态
	OrderSubmitStatusType ordersubmitstatus(){return m_orderSubmitStatus;}
	void set_ordersubmitstatus(OrderSubmitStatusType submitStatus){m_orderSubmitStatus = submitStatus;}

	///报单提示序号
	int notifysequence(){return m_notifySequence;}
	void set_notifysequence(int notifySeq){m_notifySequence = notifySeq;}

	///交易日
	const string& tradingday(){return m_tradingDay;}
	void set_tradingday(const string& tradingDay){m_tradingDay = tradingDay;}

	///结算编号
	int settlementid(){return m_settlementId;}
	void set_settlementid(int settlementId){m_settlementId = settlementId;}

	///报单编号
	const string& ordersysid(){return m_orderSysId;}
	void set_ordersysid(const string& ordSysId){m_orderSysId = ordSysId;}

	///报单来源
	OrderSourceType ordersource(){return m_orderSource;}
	void set_ordersource(OrderSourceType ordSrc){m_orderSource = ordSrc;}

	///报单状态
	OrderStatusType orderstatus(){return m_orderStatus;}
	void set_orderstatus(OrderStatusType ordStatus){m_orderStatus = ordStatus;}
	
	///报单类型
	OrderTypeType ordertype(){return m_orderType;}
	void set_ordertype(OrderTypeType ordType){m_orderType = ordType;}

	///今成交数量
	int volumetraded(){return m_volumeTraded;}
	void set_volumetraded(int volTraded){m_volumeTraded = volTraded;}

	///剩余数量
	int volumetotal(){return m_volumeTotal;}
	void set_volumetotal(int volTotal){m_volumeTotal = volTotal;}

	///报单日期
	const string& insertdate(){return m_insertDate;}
	void set_insertdate(const string& insertDate){m_insertDate = insertDate;}

	///委托时间
	const string& inserttime(){return m_insertTime;}
	void set_inserttime(const string& insertTime){m_insertTime = insertTime;}

	///激活时间
	const string& activetime(){return m_activeTime;}
	void set_activetime(const string& activeTime){m_activeTime = activeTime;}

	///挂起时间
	const string& suspendtime(){return m_suspendTime;}
	void set_suspendtime(const string& suspendTime){m_suspendTime = suspendTime;}

	///最后修改时间
	const string& updatetime(){return m_updateTime;}
	void set_updatetime(const string& updateTime){m_updateTime = updateTime;}

	///撤销时间
	const string& canceltime(){return m_cancelTime;}
	void set_canceltime(const string& cancelTime){m_cancelTime = cancelTime;}

	///最后修改交易所交易员代码
	const string& activetraderid(){return m_activateTraderId;}
	void set_activetraderid(const string& activeTraderId){m_activateTraderId = activeTraderId;}

	///结算会员编号
	const string& clearingpartid(){return m_clearingPartId;}
	void set_clearingpartid(const string& clearingPartId){m_clearingPartId = clearingPartId;}

	///序号
	int sequenceno(){return m_sequenceNo;}
	void set_sequenceno(int seqNo){m_sequenceNo = seqNo;}

	///前置编号
	int frontid(){return m_frontId;}
	void set_frontid(int frontId){m_frontId = frontId;}

	///会话编号
	int sessionid(){return m_sessionId;}
	void set_sessionid(int sessionId){m_sessionId = sessionId;}

	///用户端产品信息
	const string& userproductinfo(){return m_userProductInfo;}
	void set_userproductinfo(const string& userProdInfo){m_userProductInfo = userProdInfo;}

	///状态信息
	const string& statusmsg(){return m_statusMsg;}
	void set_statusmsg(const string& statusMsg){m_statusMsg = statusMsg;}

	///操作用户代码
	const string& activeuserid(){return m_activeUserId;}
	void set_activeuserid(const string& activeUserId){m_activeUserId = activeUserId;}

	///经纪公司报单编号
	int brokerorderseq(){return m_brokerOrderSeq;}
	void set_brokerorderseq(int brokerOrdSeq){m_brokerOrderSeq = brokerOrdSeq;}

	///相关报单
	const string& relativeordersysid(){return m_relativeOrderSysId;}
	void set_relativeordersysid(const string& relativeOrdSysId){m_relativeOrderSysId = relativeOrdSysId;}

private:

	string m_orderLocalId;
	string m_exchangeId;
	string m_participantId;
	string m_clientId;
	string m_exchangeInstId;
	string m_traderId;
	int m_installId;
	OrderSubmitStatusType m_orderSubmitStatus;
	int m_notifySequence;
	string m_tradingDay;
	int m_settlementId;
	string m_orderSysId;
	OrderSourceType m_orderSource;
	OrderStatusType m_orderStatus;
	OrderTypeType m_orderType;
	int m_volumeTraded;
	int m_volumeTotal;
	string m_insertDate;
	string m_insertTime;
	string m_activeTime;
	string m_suspendTime;
	string m_updateTime;
	string m_cancelTime;
	string m_activateTraderId;
	string m_clearingPartId;
	int m_sequenceNo;
	int m_frontId;
	int m_sessionId;
	string m_userProductInfo;
	string m_statusMsg;
	string m_activeUserId;
	int m_brokerOrderSeq;
	string m_relativeOrderSysId;
};

