#pragma once

#include <string>

using namespace std;

class CInputOrderAction
{
public:
	CInputOrderAction(void);
	~CInputOrderAction(void);

	///经纪公司代码
	const string& broderid(){ return m_brokerId; }
	void set_brokerid(const string& brokerId){ m_brokerId = brokerId; }

	///投资者代码
	const string& investorid() {return m_investorId; }
	void set_investorid(const string& investorId) { m_investorId = investorId; }

	///报单操作引用
	int orderactionref(){return m_orderActionRef;}
	void set_orderactionref(int ordActionRef){m_orderActionRef = ordActionRef;}

	///报单引用
	const string& orderref() { return m_orderRef; }
	void set_orderref(const string& ordRef){ m_orderRef = ordRef; }

	///请求编号
	int requestid(){return m_requestId;}
	void set_requestid(int reqId){m_requestId = reqId;}

	///前置编号
	int frontid(){return m_frontId;}
	void set_frontid(int frontId){m_frontId = frontId;}

	///会话编号
	int sessionid(){return m_sessionId;}
	void set_sessionid(int sessionId){m_sessionId = sessionId;}

	///交易所代码
	const string& exchangeid(){return m_exchangeId;}
	void set_exchangeid(const string& exchId){m_exchangeId = exchId;}

	///报单编号
	const string& ordersysid(){return m_orderSysId;}
	void set_ordersysid(const string& ordSysId){m_orderSysId = ordSysId;}

	///操作标志
	ActionFlagType actionflag(){return m_actionFlag;}
	void set_actionflag(ActionFlagType flagType){m_actionFlag = flagType;}

	///价格
	double limitprice(){return m_limitPrice;}
	void set_limitprice(double limitPrice){m_limitPrice = limitPrice;}

	///数量变化
	int volumechange(){return m_volumeChange;}
	void set_volumechange(int volChange){m_volumeChange = volChange;}

	///用户代码
	const string& userid() {return m_userId;}
	void set_userid(const string& userId) {m_userId = userId;}

	///合约代码
	const string& instrumentid() { return m_instrumentId; }
	void set_instrumentid(const string& instrumentId) { m_instrumentId = instrumentId; }


private:

	string m_brokerId;
	string m_investorId;
	int m_orderActionRef;
	string m_orderRef;

	int m_requestId;
	int m_frontId;
	int m_sessionId;

	string m_exchangeId;
	string m_orderSysId;

	ActionFlagType m_actionFlag;
	double m_limitPrice;
	int m_volumeChange;
	string m_userId;
	string m_instrumentId;
};

