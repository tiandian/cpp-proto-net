#include "StdAfx.h"
#include "Leg.h"

#include "Portfolio.h"


CLeg::CLeg(CPortfolio* portfolio) :
	m_pPortfolio(portfolio),
	m_legStatus(UNOPENED),
	m_orderStatus(protoc::STATUS_UNKNOWN)
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
