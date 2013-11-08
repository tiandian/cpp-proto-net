#include "StdAfx.h"
#include "WMATrendDataSet.h"
#include "globalmembers.h"
#include "OHLCRecordSet.h"

#include <ta-lib/ta_libc.h>

CWMATrendDataSet::CWMATrendDataSet(int size, int wmaParam, int maN)
	: CTaIndicatorSet(size)
{
	SetWmaParam(wmaParam);
	SetMaN(maN);

	m_arrFast = AddIndicator(IND_FAST_LINE);
	m_arrSlow = AddIndicator(IND_SLOW_LINE);
}


CWMATrendDataSet::~CWMATrendDataSet(void)
{
}

void CWMATrendDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	int lastIdx = ohlcRecordSet->GetEndIndex();
	logger.Info(boost::str(boost::format("Calculating WMA Trend: lastIdx - %d, last price - %f")
		% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));

	int outBeg = -1;
	int outNbElement = -1;
	double outFast = 0;
	double outSlow = 0;

	TA_RetCode rc = TA_WMA(lastIdx, lastIdx, (ohlcRecordSet->CloseSeries).get(), m_wmaParam, &outBeg, &outNbElement, &outFast);
	if(outBeg == lastIdx)
		m_arrFast[outBeg] = outFast;

	rc = TA_SMA(lastIdx, lastIdx, m_arrFast, m_maN, &outBeg, &outNbElement, &outSlow);
	if(outBeg == lastIdx)
	{
		m_arrSlow[outBeg] = outSlow;

		logger.Info(boost::str(boost::format("Calculated WMA Trend: fast - %.2f, slow - %.2f")
			% m_arrFast[outBeg] % m_arrSlow[outBeg]));
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot calculate MA on WMA line !!! lastIdx:%d, outBeg:%d")
			% lastIdx % outBeg));
	}

	m_lastPosition = lastIdx;
}
