#include "StdAfx.h"
#include "BollDataSet.h"


CBollDataSet::CBollDataSet(int size, int paramM, int paramP)
	: CTaIndicatorSet(size)
{
	SetM(paramM);
	SetP(paramP);

	m_arrMid = AddIndicator(IND_MID);
	m_arrTop = AddIndicator(IND_TOP);
	m_arrBottom = AddIndicator(IND_BOTTOM);
}


CBollDataSet::~CBollDataSet(void)
{
}

void CBollDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{

}
