#include "StdAfx.h"
#include "BollDataSet.h"
#include "globalmembers.h"
#include "OHLCRecordSet.h"
#include "DiffRecordSet.h"

#include <ta-lib/ta_libc.h>

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
	int lastIdx = ohlcRecordSet->GetEndIndex();
	logger.Info(boost::str(boost::format("Calculating BOLL with OHLC RecordSet: lastIdx - %d, last price - %f")
		% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));

	//CalculateTA(ohlcRecordSet, lastIdx);
	CalculateRaw((ohlcRecordSet->CloseSeries).get(), lastIdx);

	m_lastPosition = lastIdx;
}

void CBollDataSet::Calculate( CDiffRecordSet* diffRecordSet )
{
	int lastIdx = diffRecordSet->GetEndIndex();
	logger.Info(boost::str(boost::format("Calculating BOLL with DiffRecordSet: lastIdx - %d, last price - %f")
		% lastIdx % (diffRecordSet->DiffSeries)[lastIdx]));

	CalculateRaw((diffRecordSet->DiffSeries).get(), lastIdx);

	m_lastPosition = lastIdx;
}

void CBollDataSet::CalculateRaw( double* pxArray, int lastIdx )
{
	int outBeg = -1;
	int outNbElement = -1;
	double outSma = 0;
	double outStdDev = 0;

	TA_RetCode rc = TA_SMA(lastIdx, lastIdx, pxArray, m_paramM, &outBeg, &outNbElement, &outSma);
	if(outBeg == lastIdx)
		m_arrMid[outBeg] = outSma;

	rc = TA_STDDEV(lastIdx, lastIdx, pxArray, m_paramM, (double)m_paramP,
		&outBeg, &outNbElement, &outStdDev);
	if(outBeg == lastIdx)
	{
		m_arrTop[outBeg] = outSma + outStdDev;
		m_arrBottom[outBeg] = outSma - outStdDev;

		logger.Info(boost::str(boost::format("Calculated BOLL values: mid - %.2f, top - %.2f, bottom - %.2f")
			% m_arrMid[outBeg] % m_arrTop[outBeg] % m_arrBottom[outBeg] ));
	}
	else
	{
		logger.Warning(boost::str(boost::format("Cannot calculate STDDEV !!! lastIdx:%d, outBeg:%d")
			% lastIdx % outBeg));
	}
}

void CBollDataSet::CalculateTA( double* pxArray, int lastIdx )
{
	int outBeg = -1;
	int outNbElement = -1;
	double outMid = 0;
	double outTop = 0;
	double outBottom = 0;

	TA_RetCode rc = TA_BBANDS(lastIdx, lastIdx, pxArray, 
		m_paramM, 2.0, 2.0, TA_MAType_SMA, &outBeg, &outNbElement, &outTop, &outMid, &outBottom);

	if(outBeg == lastIdx)
	{
		m_arrTop[outBeg] = outTop;
		m_arrBottom[outBeg] = outBottom;
		m_arrMid[outBeg] = outMid;

		logger.Info(boost::str(boost::format("Calculated BOLL values: mid - %.2f, top - %.2f, bottom - %.2f")
			% m_arrMid[outBeg] % m_arrTop[outBeg] % m_arrBottom[outBeg] ));
	}
}
