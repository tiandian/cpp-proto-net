#include "StdAfx.h"
#include "MACDDataSet.h"
#include "OHLCRecordSet.h"
#include "globalmembers.h"

#include <ta-lib/ta_libc.h>

#define PERIOD_TO_FACTOR( per ) ((double)2.0 / ((double)(per + 1)))

CMACDDataSet::CMACDDataSet(int size, int paramShort, int paramLong, int paramM)
	: CTaIndicatorSet(size)
	, m_seedCalcMethod(INPUT_SEED)
	, m_seedShort(0)
	, m_seedLong(0)
	, m_seedSignal(0)
{
	SetShort(paramShort);
	SetLong(paramLong);
	SetM(paramM);

	m_arrMacd = AddIndicator(IND_MACD);
	m_arrMacdSignal = AddIndicator(IND_MACD_SIGNAL);
	m_arrMacdHist = AddIndicator(IND_MACD_HIST);

	//TA_RetCode rc = TA_SetCompatibility(TA_COMPATIBILITY_METASTOCK);

	if(m_seedCalcMethod == INPUT_SEED)
	{
		m_arrEmaShort = DataSeries(new double[m_size]);
		m_arrEmaLong = DataSeries(new double[m_size]);
	}
}


CMACDDataSet::~CMACDDataSet(void)
{
}

void CMACDDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	if(m_seedCalcMethod == INPUT_SEED)
	{
		CalculateBaseOnSeed(ohlcRecordSet);
	}
	else
	{
		CalculateBaseOnHistData(ohlcRecordSet);
	}
}

void CalculateEMA(int startIdx, int endIdx, double factor, double seed, double* inRealArr, double* outRealArr)
{
	double prevMA = seed;
	int today = startIdx;

	while( today <= endIdx )
	{
		prevMA = ( (inRealArr[today++] - prevMA) * factor) + prevMA;
		outRealArr[today++] = prevMA;
	}
}

void CMACDDataSet::CalculateBaseOnSeed( COHLCRecordSet* ohlcRecordSet )
{
	int endIdx = ohlcRecordSet->GetEndIndex();
	int beginIdx = ohlcRecordSet->GetBeginIndex();

	if(m_lastPosition < 1)
	{
		// Calculate EMA(Close, 12)
		CalculateEMA(beginIdx, endIdx, m_ShortEmaFactor, m_seedShort, 
			&(ohlcRecordSet->CloseSeries)[beginIdx], &m_arrEmaShort[beginIdx]);
		// Calculate EMA(Close, 26)
		CalculateEMA(beginIdx, endIdx, m_LongEmaFactor, m_seedLong, 
			&(ohlcRecordSet->CloseSeries)[beginIdx], &m_arrEmaLong[beginIdx]);
		// Calculate MACD (DIFF)
		for(int idx = beginIdx; idx <= endIdx; ++idx)
		{
			m_arrMacd[idx] = m_arrEmaShort[idx] - m_arrEmaLong[idx];
		}
		// Calculate MACD Signal (DEA)
		CalculateEMA(beginIdx, endIdx, m_SignalEmaFactor, m_seedSignal,
			&m_arrMacd[beginIdx], &m_arrMacdSignal[beginIdx]);
		// Calculate MACD Hist
		for(int idx = beginIdx; idx <= endIdx; ++idx)
		{
			m_arrMacdHist[idx] = (double)2.0 * (m_arrMacd[idx] - m_arrMacdSignal[idx]);
		}
	}
	else
	{
		// Calculate EMA(Close, 12)
		CalculateEMA(endIdx, endIdx, m_ShortEmaFactor, m_arrEmaShort[endIdx - 1], 
			&(ohlcRecordSet->CloseSeries)[endIdx], &m_arrEmaShort[endIdx]);
		// Calculate EMA(Close, 26)
		CalculateEMA(endIdx, endIdx, m_LongEmaFactor, m_arrEmaLong[endIdx - 1], 
			&(ohlcRecordSet->CloseSeries)[endIdx], &m_arrEmaLong[endIdx]);
		// Calculate MACD (DIFF)
		m_arrMacd[endIdx] = m_arrEmaShort[endIdx] - m_arrEmaLong[endIdx];
		// Calculate MACD Signal (DEA)
		CalculateEMA(endIdx, endIdx, m_SignalEmaFactor, m_arrMacdSignal[endIdx - 1],
			&m_arrMacd[endIdx], &m_arrMacdSignal[endIdx]);
		// Calculate MACD Hist
		m_arrMacdHist[endIdx] = (double)2.0 * (m_arrMacd[endIdx] - m_arrMacdSignal[endIdx]);
	}

	m_lastPosition = endIdx;
}

void CMACDDataSet::CalculateBaseOnHistData( COHLCRecordSet* ohlcRecordSet )
{
	int outBeg = -1;
	int outNbElement = -1;

	int lastIdx = ohlcRecordSet->GetEndIndex();
	logger.Info(boost::str(boost::format("Calculating OHLC RecordSet: lastIdx - %d, last - %f")
		% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));
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

		logger.Info(boost::str(boost::format("Calculated MACD values: macd - %f, signal - %f, hist - %f")
			% outMacd % outMacdSignal % outMacdHist ));
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

void CMACDDataSet::SetShort( int val )
{
	m_paramShort = val;
	m_ShortEmaFactor = PERIOD_TO_FACTOR(m_paramShort);
}

void CMACDDataSet::SetLong( int val )
{
	m_paramLong = val;
	m_LongEmaFactor = PERIOD_TO_FACTOR(m_paramLong);
}

void CMACDDataSet::SetM( int val )
{
	m_paramM = val;
	m_SignalEmaFactor = PERIOD_TO_FACTOR(m_paramM);
}

