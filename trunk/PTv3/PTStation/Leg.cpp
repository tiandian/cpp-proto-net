#include "StdAfx.h"
#include "Leg.h"


CLeg::CLeg(int legId, const entity::LegItem& srcLegItem)
	: m_legId(legId)
{
	m_symbol = srcLegItem.symbol();
	m_ratio = srcLegItem.ratio();
	m_side = srcLegItem.side();
	m_isPreferred = srcLegItem.ispreferred();

	m_legUpdate.set_symbol(m_symbol);
}


CLeg::~CLeg(void)
{
}

const entity::LegUpdateItem& CLeg::GetUpdated()
{
	m_legUpdate.set_last(m_last);
	m_legUpdate.set_ask(m_ask);
	m_legUpdate.set_asksize(m_askSize);
	m_legUpdate.set_bid(m_bid);
	m_legUpdate.set_bidsize(m_bidSize);
	m_legUpdate.set_status(m_status);
	return m_legUpdate;
}
