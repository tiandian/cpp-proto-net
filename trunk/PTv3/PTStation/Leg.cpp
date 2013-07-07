#include "StdAfx.h"
#include "Leg.h"


CLeg::CLeg(int legId, const entity::LegItem& srcLegItem, entity::LegUpdateItem* pUpdateItem)
	: m_legId(legId)
	, m_legUpdate(pUpdateItem)
{
	m_symbol = srcLegItem.symbol();
	m_ratio = srcLegItem.ratio();
	m_side = srcLegItem.side();
	m_isPreferred = srcLegItem.ispreferred();
}


CLeg::~CLeg(void)
{
}

void CLeg::GetUpdated()
{
	m_legUpdate->set_last(m_last);
	m_legUpdate->set_ask(m_ask);
	m_legUpdate->set_asksize(m_askSize);
	m_legUpdate->set_bid(m_bid);
	m_legUpdate->set_bidsize(m_bidSize);
	m_legUpdate->set_status(m_status);
}
