#include "StdAfx.h"
#include "WATRStopDataSet.h"
#include "globalmembers.h"
#include "OHLCRecordSet.h"


CWATRStopDataSet::CWATRStopDataSet(int size, int atrPeriod)
	: CTaIndicatorSet(size)
{
	SetAtrPeriod(atrPeriod);

	m_arrTrend = AddIndicator(IND_WATR_TREND);
	m_arrSummitPx = AddIndicator(IND_WATR_SUMMIT_PRICE);
	m_arrStopPx = AddIndicator(IND_WATR_STOP);
	m_arrWatr = AddIndicator(IND_WATR_VAL);
}


CWATRStopDataSet::~CWATRStopDataSet(void)
{
}

void CWATRStopDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	int nbElements = ohlcRecordSet->NbElements();
	int lastIdx = ohlcRecordSet->GetEndIndex();

	//logger.Debug(boost::str(boost::format("Calculating WATR Stop: lastIdx - %d, last price - %.2f")
	//	% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));

	if(lastIdx > m_lastPosition)
	{
		if(m_lastPosition > 0)
		{
			CalcWATRStopLast(ohlcRecordSet);
		}
		else // the first time, calculate and fill all 
		{
			int outBegin = -1;
			int outNbElements = 0;
			CalcWATRStop(0, lastIdx, ohlcRecordSet, &outBegin, &outNbElements);
			logger.Debug(boost::str(boost::format("The first time calculating WATR Stop: outBegin - %d, outNbElements - %d")
				% outBegin % outNbElements));
		}

		logger.Debug(boost::str(boost::format("Calculated WATR Stop: trend - %.0f, SummitPx - %.3f, StopPx - %.3f, watr - %.3f")
			% m_arrTrend[lastIdx] % m_arrSummitPx[lastIdx] % m_arrStopPx[lastIdx] % m_arrWatr[lastIdx]));
		
		m_lastPosition = lastIdx;
	}
}

void CWATRStopDataSet::CalcWATR( COHLCRecordSet* ohlcRecordSet, int idx )
{
	double totalRange = 0.0;
	for(int i = 1; i <= m_atrPeriod; ++i)
	{
		int pos = idx - i;
		if(pos >= 0)
		{
			double dk = 1.0 + (double(m_atrPeriod - i)/m_atrPeriod);
			double range = ohlcRecordSet->HighSeries[pos] - ohlcRecordSet->LowSeries[pos];
			totalRange += dk * fabs(range);
		}
	}

	double watr = totalRange / m_atrPeriod;
	m_arrWatr[idx] = watr;
}

void CWATRStopDataSet::CalcWATRStop( int beginIdx, int endIdx, COHLCRecordSet* ohlcRecordSet, int* outBeg, int* outNbElem )
{
	int dataBeg = ohlcRecordSet->GetBeginIndex();
	int dataEnd = ohlcRecordSet->GetEndIndex();
	
	// The first nbElement
	int currIdx = dataBeg > beginIdx ? dataBeg : beginIdx;
	currIdx += m_atrPeriod;
	int loopEndIdx = endIdx <= dataEnd ? endIdx : dataEnd; 

	*outBeg = -1;
	*outNbElem = 0;

	if(currIdx <= loopEndIdx)
	{
		CalcWATR(ohlcRecordSet, currIdx);
		m_arrSummitPx[currIdx] = ohlcRecordSet->CloseSeries[currIdx];

		if(ohlcRecordSet->CloseSeries[currIdx] >= ohlcRecordSet->OpenSeries[currIdx])
		{
			m_arrTrend[currIdx] = 1;
			m_arrStopPx[currIdx] = m_arrSummitPx[currIdx] - m_arrWatr[currIdx];
		}
		else
		{
			m_arrTrend[currIdx] = -1;
			m_arrStopPx[currIdx] = m_arrSummitPx[currIdx] + m_arrWatr[currIdx];
		}

		*outBeg = currIdx;
		*outNbElem = 1;
	}
	
	// Move to next
	++currIdx;
	while (currIdx <= loopEndIdx)
	{
		++(*outNbElem);
		CalcWATRStopAt(ohlcRecordSet, currIdx);
		++currIdx;
	}
}

void CWATRStopDataSet::CalcWATRStopLast( COHLCRecordSet* ohlcRecordSet )
{
	int lastIdx = ohlcRecordSet->GetEndIndex();
	CalcWATRStopAt(ohlcRecordSet, lastIdx);
}

void CWATRStopDataSet::CalcWATRStopAt( COHLCRecordSet* ohlcRecordSet, int currIdx )
{
	CalcWATR(ohlcRecordSet, currIdx);
	double close = ohlcRecordSet->CloseSeries[currIdx];
	double open = ohlcRecordSet->OpenSeries[currIdx];
	double watr = m_arrWatr[currIdx];
	double trend = m_arrTrend[currIdx - 1];
	double price = m_arrSummitPx[currIdx - 1];
	double stopPx = 0;
	if(trend > 0) // long
	{
		if ( close > price) price = close;
		stopPx = price - watr;

		if ((open < stopPx || close <= open)
				&& close < stopPx) 
		{
			price = close;
			stopPx = price + watr;
			trend = -1;
		}
	}else	// short
	{
		if ( close < price) price = close;
		stopPx = price + watr;
		
		if ((open > stopPx || close >= open) 
				&& close > stopPx) 
		{
			price = close;
			stopPx = price - watr;
			trend = 1;
		}
	}
	m_arrTrend[currIdx] = trend;
	m_arrSummitPx[currIdx] = price;
	m_arrStopPx[currIdx] = stopPx;
}
