#pragma once

#include "TradeEnumDefinition.h"

#include <string>

using namespace std;

class CInputOrder
{
public:
	CInputOrder(void);
	~CInputOrder(void);

	///经纪公司代码
	const string& broderid(){ return m_brokerId; }
	void set_brokerid(const string& brokerId){ m_brokerId = brokerId; }

	///投资者代码
	const string& investorid() {return m_investorId; }
	void set_investorid(const string& investorId) { m_investorId = investorId; }

	///合约代码
	const string& instrumentid() { return m_instrumentId; }
	void set_instrumentid(const string& instrumentId) { m_instrumentId = instrumentId; }

	///报单引用
	const string& orderref() { return m_orderRef; }
	void set_orderref(const string& ordRef){ m_orderRef = ordRef; }

	///用户代码
	const string& userid() {return m_userId;}
	void set_userid(const string& userId) {m_userId = userId;}

	///报单价格条件
	OrderPriceTypeType orderpricetype(){return m_orderPriceType;}
	void set_orderpricetype(OrderPriceTypeType orderPriceType){m_orderPriceType = orderPriceType;}

	///买卖方向
	TradeDirectionType direction(){return m_direction;}
	void set_direction(TradeDirectionType tradeDirection){m_direction = tradeDirection;}

	///组合开平标志
	const string& comboffsetflag() {return m_combOffsetFlag;}
	void set_comboffsetflag(const string& combOffsetFlag) {m_combOffsetFlag = combOffsetFlag;}

	///组合投机套保标志
	const string& combhedgeflag() {return m_combHedgeFlag;}
	void set_combhedgeflag(const string& combHedgeFlag) {m_combHedgeFlag = combHedgeFlag;}
	
	///价格
	double limitprice() {return m_limitPrice;}
	void set_limitprice(double limitPrice){m_limitPrice = limitPrice;}

	///数量
	int volumetotaloriginal(){return m_volumeTotalOriginal;}
	void set_volumetotaloriginal(int volTotalOrig){m_volumeTotalOriginal = volTotalOrig;}

	///有效期类型
	TimeConditionType timecondition(){return m_timeCondition;}
	void set_timecondition(TimeConditionType timeCond){m_timeCondition = timeCond;}

	///GTD日期
	const string& gtddate(){return m_gtdDate;}
	void set_gtddate(const string& gtdDate){m_gtdDate = gtdDate;}

	///成交量类型
	VolumeConditionType volumecondition(){return m_volumeCondition;}
	void set_volumecondition(VolumeConditionType volCond){m_volumeCondition = volCond;}

	///最小成交量
	int minvolume(){return m_minVolume;}
	void set_minvolume(int minVol){m_minVolume = minVol;}

	///触发条件
	ContingentConditionType contingentcondition(){return m_contingentCondition;}
	void set_contingentcondition(ContingentConditionType triggerCond){m_contingentCondition = triggerCond;}

	///止损价
	double stopprice(){return m_stopPrice;}
	void set_stopprice(double stopPx){m_stopPrice = stopPx;}

	///强平原因
	ForceCloseReasonType forceclosereason(){return m_forceCloseReason;}
	void set_forceclosereason(ForceCloseReasonType closeReason){m_forceCloseReason = closeReason;}

	///自动挂起标志
	bool isautosuspend(){return m_isAutoSuspend;}
	void set_isautosuspend(bool autoSuspend){m_isAutoSuspend = autoSuspend;}

	///业务单元
	const string& businessunit(){return m_businessUnit;}
	void set_businessunit(const string& buszUnit){m_businessUnit = buszUnit;}

	///请求编号
	int requestid(){return m_requestId;}
	void set_requestid(int reqId){m_requestId = reqId;}

	///用户强评标志
	bool userforceclose(){return m_userForceClose;}
	void set_userforceclose(bool userForceClose){m_userForceClose = userForceClose;}

private:
	string m_brokerId;
	string m_investorId;
	string m_instrumentId;
	string m_orderRef;
	string m_userId;
	OrderPriceTypeType m_orderPriceType;
	TradeDirectionType m_direction;
	string m_combOffsetFlag;
	string m_combHedgeFlag;
	double m_limitPrice;
	int m_volumeTotalOriginal;
	TimeConditionType m_timeCondition;
	string m_gtdDate;
	VolumeConditionType m_volumeCondition;
	int m_minVolume;
	ContingentConditionType m_contingentCondition;
	double m_stopPrice;
	ForceCloseReasonType m_forceCloseReason;
	bool m_isAutoSuspend;
	string m_businessUnit;
	int m_requestId;
	bool m_userForceClose;
};

