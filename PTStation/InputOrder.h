#pragma once

#include "entity/trade.pb.h"
#include "ThostTraderApi/ThostFtdcTraderApi.h"

class CInputOrder
{
public:
	CInputOrder(void);
	~CInputOrder(void);

	const string& Symbol(){ return m_symbol; }

	void set_brokerid(const string& brokerId);
	///投资者代码
	void set_investorid(const string& investorId);
	///合约代码
	void set_symbol(const string& symbol);
	///报单引用
	void set_orderref(const string& orderRef);
	const string& OrderRef(){ return m_orderRef; }
	///用户代码
	void set_userid(const string& userId){}
	///报单价格条件: 限价
	void set_orderpricetype(trade::OrderPriceTypeType ordPriceType);
	///买卖方向: 
	void set_direction(trade::TradeDirectionType tradeDirection);
	trade::TradeDirectionType Direction(){ return m_direction; }
	///组合开平标志: 开仓
	void set_comboffsetflag(const string& offsetFlag);
	const string& OffsetFlag(){ return m_offsetFlag; }
	///组合投机套保标志
	void set_combhedgeflag(const string& hedgeFlag);
	///价格
	void set_limitprice(double lmtPx);
	double LimitPrice(){ return m_innerOrderItem.LimitPrice; }
	///数量: 1
	void set_volumetotaloriginal(int volTotal);
	int VolumeTotalOriginal(){ return m_innerOrderItem.VolumeTotalOriginal; }
	///有效期类型: 当日有效
	void set_timecondition(trade::TimeConditionType timeConditionType);
	///GTD日期
	void set_gtddate(const string& gtdDate){}
	///成交量类型: 任何数量
	void set_volumecondition(trade::VolumeConditionType volConditionType);
	///最小成交量: 1
	void set_minvolume(int minVol);
	///触发条件: 立即
	void set_contingentcondition(trade::ContingentConditionType contingentCond);
	///止损价
	void set_stopprice(double stopPx){}
	///强平原因: 非强平
	void set_forceclosereason(trade::ForceCloseReasonType forceCloseReason);
	///自动挂起标志: 否
	void set_isautosuspend(bool isAutoSuspend);
	///业务单元
	void set_businessunit(const string& busiUnit){}
	///请求编号
	void set_requestid(int reqId);
	///用户强评标志: 否
	void set_userforceclose(bool userForceClose);

	CThostFtdcInputOrderField& InnerOrder(){ return m_innerOrderItem; }

private:
	string m_symbol;
	string m_orderRef;
	string m_offsetFlag;
	trade::TradeDirectionType m_direction;

	CThostFtdcInputOrderField m_innerOrderItem;
};



