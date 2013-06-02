#include "StdAfx.h"
#include "Leg.h"

#include "Portfolio.h"


CLeg::CLeg(CPortfolio* portfolio) :
	m_pPortfolio(portfolio),
	m_legStatus(UNOPENED),
	m_orderStatus(protoc::STATUS_UNKNOWN),
	m_openOrderPriceType(protoc::LIMIT_PRICE),
	m_openLimitPrice(0),
	m_closeOrderPriceType(protoc::LIMIT_PRICE),
	m_closeLimitPrice(0),
	m_cost(0)
{
}


CLeg::~CLeg(void)
{
}

int CLeg::GetQuantity()
{
	int portfolioQuantity = m_pPortfolio->GetQuantity();
	return portfolioQuantity * m_ratio;
}

const char* CLeg::GetSideText()
{
	static char sideText[][6] = { "NET", "LONG", "SHORT" };
	return sideText[m_side - protoc::NET];
}

const char* CLeg::GetStatusText()
{
	static char statusText[][11] = {
										"UNOPENED",
										"IS_OPENING",
										"OPENED",
										"IS_CLOSING"
									};
	return statusText[m_legStatus];
}

const char* CLeg::GetOrderStatusText()
{
	static char orderStatusText[][25] = {
		"ALL_TRADED",
		"PART_TRADED_QUEUEING",
		"PART_TRADED_NOT_QUEUEING",
		"NO_TRADE_QUEUEING",
		"NO_TRADE_NOT_QUEUEING",
		"ORDER_CANCELED",
		"STATUS_UNKNOWN",
		"ORDER_NOT_TOUCHED",
		"ORDER_TOUCHED"
									};
	return orderStatusText[m_orderStatus < protoc::STATUS_UNKNOWN ? m_orderStatus - protoc::ALL_TRADED : m_orderStatus - protoc::STATUS_UNKNOWN + 6];
}

const char* CLeg::GetOrderSubmitStatusText()
{
	static char submitStatusText[][17] = {
		"INSERT_SUBMITTED",
		"CANCEL_SUBMITTED",
		"MODIFY_SUBMITTED",
		"ACCEPTED",
		"INSERT_REJECTED",
		"CANCEL_REJECTED",
		"MODIFY_REJECTED",
		"UNSUBMITTED"
	};
	return m_orderRef.empty() ? submitStatusText[7] : 
		submitStatusText[m_orderSubmitStatus - protoc::INSERT_SUBMITTED];
}


