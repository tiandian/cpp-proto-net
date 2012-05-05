#pragma once

#include <string>

using namespace std;

class CTrade
{
public:
	CTrade(void);
	~CTrade(void);

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

	///交易所代码
	const string& exchangeid(){return m_exchangeId;}
	void set_exchangeid(const string& exchId){m_exchangeId = exchId;}

	///成交编号
	const string& tradeid(){return m_tradeId;}
	void set_tradeid(const string& tradeId){m_tradeId = tradeId;}

	///买卖方向
	TradeDirectionType direction(){return m_direction;}
	void set_direction(TradeDirectionType tradeDirection){m_direction = tradeDirection;}

	///报单编号
	const string& ordersysid(){return m_orderSysId;}
	void set_ordersysid(const string& ordSysId){m_orderSysId = ordSysId;}

	///会员代码
	const string& participantid(){return m_participantId;}
	void set_participantid(const string& partId){m_participantId = partId;}

	///客户代码
	const string& clientid(){return m_clientId;}
	void set_clientid(const string& clientId){m_clientId = clientId;}

	///交易角色
	TradingRoleType tradingrole(){return m_tradingRole;}
	void set_tradingrole(TradingRoleType role){m_tradingRole = role;}

	///合约在交易所的代码
	const string& exchangeinstid(){return m_exchangeInstId;}
	void set_exchangeinstid(const string& exchInstId){m_exchangeInstId = exchInstId;}

	///组合开平标志
	const string& offsetflag() {return m_combOffsetFlag;}
	void set_offsetflag(const string& combOffsetFlag) {m_combOffsetFlag = combOffsetFlag;}

	///组合投机套保标志
	const string& hedgeflag() {return m_combHedgeFlag;}
	void set_hedgeflag(const string& combHedgeFlag) {m_combHedgeFlag = combHedgeFlag;}

	///价格
	double price(){return m_price;}
	void set_price(double px){m_price = px;}

	///数量
	int volume(){return m_volume;}
	void set_volume(int vol){m_volume = vol;}

	///成交时期
	const string& tradedate(){return m_tradeDate;}
	void set_tradedate(const string& tradeDate){m_tradeDate = tradeDate;}

	///成交时间
	const string& tradetime(){return m_tradeTime;}
	void set_tradetime(const string& tradeTime){m_tradeTime = tradeTime;}

	///成交类型
	TradeTypeType tradetype(){return m_tradeType;}
	void set_tradetype(TradeTypeType tradeType){m_tradeType = tradeType;}

	///成交价来源
	PriceSourceType pricesource(){return m_priceSource;}
	void set_pricesource(PriceSourceType priceSource){m_priceSource = priceSource;}

	///交易所交易员代码
	const string& traderid(){return m_traderId;}
	void set_traderid(const string& traderId){m_traderId = traderId;}

	///本地报单编号
	const string& orderlocalid(){return m_orderLocalId;}
	void set_orderlocalid(const string& orderLocalId){m_orderLocalId = orderLocalId;}

	///结算会员编号
	const string& clearingpartid(){return m_clearingPartId;}
	void set_clearingpartid(const string& clearingPartId){m_clearingPartId = clearingPartId;}

	///业务单元
	const string& businessunit(){return m_businessUnit;}
	void set_businessunit(const string& buszUnit){m_businessUnit = buszUnit;}

	///序号
	int sequenceno(){return m_sequenceNo;}
	void set_sequenceno(int seqNo){m_sequenceNo = seqNo;}
	
	///交易日
	const string& tradingday(){return m_tradingDay;}
	void set_tradingday(const string& tradingDay){m_tradingDay = tradingDay;}

	///结算编号
	int settlementid(){return m_settlementId;}
	void set_settlementid(int settlementId){m_settlementId = settlementId;}

	///经纪公司报单编号
	int brokerorderseq(){return m_brokerOrderSeq;}
	void set_brokerorderseq(int brokerOrdSeq){m_brokerOrderSeq = brokerOrdSeq;}


private:
	
	string m_brokerId;
	string m_investorId;
	string m_instrumentId;
	string m_orderRef;
	string m_userId;
	string m_exchangeId;
	string m_tradeId;
	TradeDirectionType m_direction;
	string m_orderSysId;
	string m_participantId;
	string m_clientId;
	TradingRoleType m_tradingRole;
	string m_exchangeInstId;
	string m_combOffsetFlag;
	string m_combHedgeFlag;
	double m_price;
	int m_volume;
	string m_tradeDate;
	string m_tradeTime;
	TradeTypeType m_tradeType;
	PriceSourceType m_priceSource;
	string m_traderId;
	string m_orderLocalId;
	string m_clearingPartId;
	string m_businessUnit;
	string m_sequenceNo;
	string m_tradingDay;
	int m_settlementId;
	int m_brokerOrderSeq;
};

