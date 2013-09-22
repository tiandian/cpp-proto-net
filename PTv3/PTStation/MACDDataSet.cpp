#include "StdAfx.h"
#include "MACDDataSet.h"
#include "OHLCRecordSet.h"

#include <ta_libc.h>

CMACDDataSet::CMACDDataSet(unsigned int size, double paramShort, double paramLong, double paramM)
	: CTaIndicatorSet(size)
	, m_paramShort(paramShort)
	, m_paramLong(paramLong)
	, m_paramM(paramM)
{
	m_arrMacd = AddIndicator(IND_MACD);
	m_arrMacdSignal = AddIndicator(IND_MACD_SIGNAL);
	m_arrMacdHist = AddIndicator(IND_MACD_HIST);
}


CMACDDataSet::~CMACDDataSet(void)
{
}

void CMACDDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	int outBeg = -1;
	int outNbElement = -1;

	int lastIdx = ohlcRecordSet->GetLastIndex();

	if(m_lastPosition > 0)
	{
		double outMacd = 0;
		double outMacdSignal = 0;
		double outMacdHist = 0;

		TA_RetCode rc = TA_MACD(lastIdx, lastIdx, (ohlcRecordSet->CloseSeries).get(), m_paramShort, m_paramLong, m_paramM, 
			&outBeg, &outNbElement, &outMacd, &outMacdSignal, &outMacdHist);

		m_arrMacd[outBeg] = outMacd;
		m_arrMacdSignal[outBeg] = outMacdSignal;
		m_arrMacdHist[outBeg] = outMacdHist;
	}
	else	// the first time, calculate and fill all 
	{
		int lookback = TA_MACD_Lookback(m_paramShort, m_paramLong, m_paramM);
		TA_RetCode rc = TA_MACD(0, lastIdx, (ohlcRecordSet->CloseSeries).get(), m_paramShort, m_paramLong, m_paramM, 
			&outBeg, &outNbElement, 
			m_arrMacd + lookback, 
			m_arrMacdSignal + lookback, 
			m_arrMacdHist + lookback);
	}

	m_lastPosition = lastIdx;
}
