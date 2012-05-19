#pragma once

#include <string>

using namespace std;

class CAccountInfo
{
public:
	CAccountInfo(void);
	~CAccountInfo(void);

	///经纪公司代码
	const std::string brokerid() const { return m_brokerId; }
	void set_brokerid(std::string val) { m_brokerId = val; }

	///投资者帐号
	std::string accountid() const { return m_accountid; }
	void set_accountid(std::string val) { m_accountid = val; }

	///上次质押金额
	double premortgage() const { return m_premortgage; }
	void set_premortgage(double val) { m_premortgage = val; }

	///上次信用额度
	double precredit() const { return m_precredit; }
	void set_precredit(double val) { m_precredit = val; }

	///上次存款额
	double predeposit() const { return m_predeposit; }
	void set_predeposit(double val) { m_predeposit = val; }

	///上次结算准备金
	double prebalance() const { return m_prebalance; }
	void set_prebalance(double val) { m_prebalance = val; }

	///上次占用的保证金
	double premargin() const { return m_premargin; }
	void set_premargin(double val) { m_premargin = val; }

	///利息基数
	double interestbase() const { return m_interestbase; }
	void set_interestbase(double val) { m_interestbase = val; }

	///利息收入
	double interest() const { return m_interest; }
	void set_interest(double val) { m_interest = val; }

	///入金金额
	double deposit() const { return m_deposit; }
	void set_deposit(double val) { m_deposit = val; }

	///出金金额
	double withdraw() const { return m_withdraw; }
	void set_withdraw(double val) { m_withdraw = val; }

	///冻结的保证金
	double frozenmargin() const { return m_frozenmargin; }
	void set_frozenmargin(double val) { m_frozenmargin = val; }

	///冻结的资金
	double frozencash() const { return m_frozencash; }
	void set_frozencash(double val) { m_frozencash = val; }

	///冻结的手续费
	double frozencommission() const { return m_frozencommission; }
	void set_frozencommission(double val) { m_frozencommission = val; }

	///当前保证金总额
	double currmargin() const { return m_currmargin; }
	void set_currmargin(double val) { m_currmargin = val; }

	///资金差额
	double cashin() const { return m_cashin; }
	void set_cashin(double val) { m_cashin = val; }

	///手续费
	double commission() const { return m_commission; }
	void set_commission(double val) { m_commission = val; }

	///平仓盈亏
	double closeprofit() const { return m_closeprofit; }
	void set_closeprofit(double val) { m_closeprofit = val; }

	///持仓盈亏
	double positionprofit() const { return m_positionprofit; }
	void set_positionprofit(double val) { m_positionprofit = val; }

	///期货结算准备金
	double balance() const { return m_balance; }
	void set_balance(double val) { m_balance = val; }
	
	///可用资金
	double available() const { return m_available; }
	void set_available(double val) { m_available = val; }

	///可取资金
	double withdrawquota() const { return m_withdrawquota; }
	void set_withdrawquota(double val) { m_withdrawquota = val; }

	///基本准备金
	double reserve() const { return m_reserve; }
	void set_reserve(double val) { m_reserve = val; }

	///交易日
	const std::string tradingday() const { return m_tradingday; }
	void set_tradingday(std::string val) { m_tradingday = val; }

	///结算编号
	int settlementid() const { return m_settlementid; }
	void set_settlementid(int val) { m_settlementid = val; }

	///信用额度
	double credit() const { return m_credit; }
	void set_credit(double val) { m_credit = val; }

	///质押金额
	double mortgage() const { return m_mortgage; }
	void set_mortgage(double val) { m_mortgage = val; }

	///交易所保证金
	double exchangemargin() const { return m_exchangemargin; }
	void set_exchangemargin(double val) { m_exchangemargin = val; }

	///投资者交割保证金
	double deliverymargin() const { return m_deliverymargin; }
	void set_deliverymargin(double val) { m_deliverymargin = val; }

	///交易所交割保证金
	double exchangedeliverymargin() const { return m_exchangedeliverymargin; }
	void set_exchangedeliverymargin(double val) { m_exchangedeliverymargin = val; }

private:
	string m_brokerId;
	string m_accountid;
	double m_premortgage;
	double	m_precredit;
	double	m_predeposit;
	double	m_prebalance;
	double	m_premargin;
	double	m_interestbase;
	double	m_interest;
	double	m_deposit;
	double	m_withdraw;
	double	m_frozenmargin;
	double	m_frozencash;
	double	m_frozencommission;
	double	m_currmargin;
	double	m_cashin;
	double	m_commission;
	double	m_closeprofit;
	double	m_positionprofit;
	double	m_balance;
	double	m_available;
	double	m_withdrawquota;
	double	m_reserve;
	string	m_tradingday;
	int	m_settlementid;
	double	m_credit;
	double	m_mortgage;
	double	m_exchangemargin;
	double	m_deliverymargin;
	double	m_exchangedeliverymargin;
};

