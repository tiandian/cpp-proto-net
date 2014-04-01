#include "StdAfx.h"
#include "ATRDataSet.h"
#include "OHLCRecordSet.h"
#include "globalmembers.h"

CATRDataSet::CATRDataSet(int size, int period)
	: CTaIndicatorSet(size)
{
	SetPeriod(period);

	m_arrATR = AddIndicator(IND_ATR);
	m_arrTrueRange = AddIndicator(IND_TRUE_RANGE);
}


CATRDataSet::~CATRDataSet(void)
{
}

void CATRDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	int nbElements = ohlcRecordSet->NbElements();
	int lastIdx = ohlcRecordSet->GetEndIndex();
	logger.Info(boost::str(boost::format("Calculating ATR with OHLC RecordSet: lastIdx - %d, last price - %f")
		% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));

	CalcTrueRange(ohlcRecordSet, nbElements, lastIdx);
	CalcATR(ohlcRecordSet, nbElements, lastIdx);

	m_lastPosition = lastIdx;
}

void CATRDataSet::CalcATR( COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx )
{
	if(nbElements >= m_period)
	{
		int startIdx = lastIdx;
		int endIdx = lastIdx - m_period + 1;
		double tmpTotal = 0;
		for(int idx = startIdx; idx >= endIdx; --idx)
		{
			tmpTotal += m_arrTrueRange[idx];
		}

		m_arrATR[lastIdx] = tmpTotal / (double)m_period;
		
		logger.Info(boost::str(boost::format("Calculated ATR: %.2f") % m_arrATR[lastIdx]));
	}
}

void CATRDataSet::CalcTrueRange( COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx )
{
	if(nbElements > 0)
	{
		if(lastIdx == 0)
		{
			m_arrTrueRange[lastIdx] = ohlcRecordSet->HighSeries[lastIdx] - ohlcRecordSet->LowSeries[lastIdx];
		}
		else
		{
			/* True high
			THighValue = Close[1];
			If(High >= Close[1])
				THighValue = High;
			*/
			double trueHigh = ohlcRecordSet->CloseSeries[lastIdx - 1];
			if(ohlcRecordSet->HighSeries[lastIdx] >= ohlcRecordSet->CloseSeries[lastIdx - 1])
				trueHigh = ohlcRecordSet->HighSeries[lastIdx];

			/* True low
			TLowValue = Close[1];
			If(Low <= Close[1])
				TLowValue = Low;
			*/
			double trueLow = ohlcRecordSet->CloseSeries[lastIdx - 1];
			if(ohlcRecordSet->LowSeries[lastIdx] <= ohlcRecordSet->CloseSeries[lastIdx - 1])
				trueLow = ohlcRecordSet->LowSeries[lastIdx];

			m_arrTrueRange[lastIdx] = trueHigh - trueLow;
		}
	}
}
