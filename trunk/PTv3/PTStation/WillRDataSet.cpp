#include "StdAfx.h"
#include "WillRDataSet.h"
#include "globalmembers.h"
#include "OHLCRecordSet.h"

#include <ta-lib/ta_libc.h>

CWillRDataSet::CWillRDataSet(int size, int risk, int atrPeriod, int breakoutLength)
	: CTaIndicatorSet(size)
	, m_dmiPeriod(14)
{
	SetRisk(risk);
	SetAtrPeriod(atrPeriod);
	SetBreakoutLength(breakoutLength);

	m_arrWR = AddIndicator(IND_WR, -1.0);
	m_arrHi = AddIndicator(IND_Donchian_Hi);
	m_arrLo = AddIndicator(IND_Donchian_Lo);
	m_arrPdi = AddIndicator(IND_PDI);
	m_arrMdi = AddIndicator(IND_MDI);
}


CWillRDataSet::~CWillRDataSet(void)
{
}

void CWillRDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	int nbElements = ohlcRecordSet->NbElements();
	int lastIdx = ohlcRecordSet->GetEndIndex();

	logger.Debug(boost::str(boost::format("Calculating Williams %%R: lastIdx - %d, last price - %.2f")
		% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));
	
	CalcWilliamsR(ohlcRecordSet, nbElements, lastIdx);
	CalcDMI(ohlcRecordSet, nbElements, lastIdx);
	CalcDonchianChannel(ohlcRecordSet, nbElements, lastIdx);

	m_lastPosition = lastIdx;
}


void CWillRDataSet::CalcWilliamsR( COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx )
{
	/* Codes in TB for reference
	// Signal Calculation Begin
	x1 = 67 + Risk;
	x2 = 33 - Risk;
	DefWRPeriod = 3 + Risk * 2;
	ActualWRPeriod = DefWRPeriod;

	Range = High - Low;
	AvgRange = Average(Range, 10);

	while (Counter < 9 && !MRO1)
	{
	if ( Abs(Open[Counter] - Close[Counter+1]) >= AvgRange * 2.0 ) 
	{
	MRO1 = True;
	}

	Counter = Counter+1;
	}

	Counter = 0;
	while (Counter < 6 && !MRO2)
	{
	if ( Abs(Close[Counter + 3] - Close[Counter]) >= AvgRange * 4.6) 
	{
	MRO2 = True;
	}

	Counter=Counter+1;
	}

	If(MRO1)
	{
	ActualWRPeriod = 3;
	}

	If(MRO2)
	{
	ActualWRPeriod = 4;
	}

	WR = PercentR(ActualWRPeriod);

	*/
	
	int outBeg = -1;
	int outNbElement = -1;

	int x1 = ASC_X1 + m_risk;
	int x2 = ASC_X2 - m_risk;
	int defWRPeriod = m_risk * 2 + 3;
	int actualWRPeriod = defWRPeriod;

	if(nbElements >= m_atrPeriod && nbElements >= defWRPeriod)
	{
		// Calculating ATR(10)
		double avgRange = GetATR(ohlcRecordSet, m_atrPeriod);

		// Test for MRO1 and MRO2
		int mro1pos = TestMRO1(ohlcRecordSet, 9, avgRange);
		
		// In case there is a bar which open price is far away the last open, give an invalid WR value to stop open position
		if(mro1pos < 3)
		{
			// Don't need to set value because default value is -1
			//m_arrWR[lastIdx] = -1; 
			return;
		}
		
		bool mro1 = mro1pos <= 9;
		bool mro2 = TestMRO2(ohlcRecordSet, 6, avgRange);

		if(mro1)
			actualWRPeriod = 3;
		if(mro2)
			actualWRPeriod = 4;

		double williamsR = -50;
		TA_RetCode rc = TA_WILLR(lastIdx, lastIdx,
			(ohlcRecordSet->HighSeries).get(), (ohlcRecordSet->LowSeries).get(), (ohlcRecordSet->CloseSeries).get(),
			actualWRPeriod, &outBeg, &outNbElement, &williamsR);  
		williamsR = 100 - fabs(williamsR);
		if(outBeg == lastIdx)
		{
			m_arrWR[outBeg] = williamsR;

			logger.Info(boost::str(boost::format("Calculated Williams %%R: %.2f") % m_arrWR[outBeg]));
		}
		else
		{
			logger.Warning(boost::str(boost::format("Cannot calculate Williams %%R !!! lastIdx:%d, outBeg:%d")
				% lastIdx % outBeg));
		}
	}
}

void CWillRDataSet::CalcDonchianChannel( COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx )
{
	if(nbElements > m_breakoutLength)
	{
		int startIdx = lastIdx - 1;
		int endIdx = lastIdx - m_breakoutLength;
		double topBound = 0, bottomBound = -1;
		for(int idx = startIdx; idx >= endIdx; --idx)
		{
			double high = ohlcRecordSet->HighSeries[idx];
			double low = ohlcRecordSet->LowSeries[idx];
			if(high > topBound)
				topBound = high;
			if(low < bottomBound || bottomBound < 0)
				bottomBound = low;
		}

		m_arrHi[lastIdx] = topBound;
		m_arrLo[lastIdx] = bottomBound;

		logger.Info(boost::str(boost::format("Calculated Donchian Channel: High - %.2f, Low - %.2f") % m_arrHi[lastIdx] % m_arrLo[lastIdx]));
	}
}

void CWillRDataSet::CalcDMI( COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx )
{
	int outBeg = -1;
	int outNbElement = -1;

	if(m_lastPosition > 0)
	{
		double outPDI = 0;
		double outMDI = 0;

		TA_RetCode rc = TA_PLUS_DI(lastIdx, lastIdx, 
			(ohlcRecordSet->HighSeries).get(),
			(ohlcRecordSet->LowSeries).get(),
			(ohlcRecordSet->CloseSeries).get(), 
			m_dmiPeriod, &outBeg, &outNbElement, &outPDI);

		if(outBeg == lastIdx)
			m_arrPdi[outBeg] = outPDI;

		rc = TA_MINUS_DI(lastIdx, lastIdx, 
			(ohlcRecordSet->HighSeries).get(),
			(ohlcRecordSet->LowSeries).get(),
			(ohlcRecordSet->CloseSeries).get(), 
			m_dmiPeriod, &outBeg, &outNbElement, &outMDI);

		if(outBeg == lastIdx)
			m_arrMdi[outBeg] = outMDI;

		logger.Info(boost::str(boost::format("Calculated DMI: PDI - %.2f, MDI - %.2f")
			% m_arrPdi[outBeg] % m_arrMdi[outBeg]));
	}
	else // the first time, calculate and fill all 
	{
		int dmi_lookback = TA_PLUS_DI_Lookback(m_dmiPeriod);
		TA_RetCode rc = TA_PLUS_DI(0, lastIdx, 
			(ohlcRecordSet->HighSeries).get(),
			(ohlcRecordSet->LowSeries).get(),
			(ohlcRecordSet->CloseSeries).get(), 
			m_dmiPeriod, &outBeg, &outNbElement,
			m_arrPdi + dmi_lookback);
		if(outBeg == dmi_lookback)
		{
			dmi_lookback = TA_MINUS_DI_Lookback(m_dmiPeriod);
			rc = TA_MINUS_DI(0, lastIdx,
				(ohlcRecordSet->HighSeries).get(),
				(ohlcRecordSet->LowSeries).get(),
				(ohlcRecordSet->CloseSeries).get(), 
				m_dmiPeriod, &outBeg, &outNbElement,
				m_arrMdi + dmi_lookback);

			if(outBeg != dmi_lookback)
			{
				logger.Warning(boost::str(boost::format("[DMI] Calculated Minus DI may NOT be correct!!!")));
			}
		}
		else
		{
			logger.Warning(boost::str(boost::format("[DMI] Calculated Plus DI may NOT be correct!!!")));
		}
	}
}

double CWillRDataSet::GetATR( COHLCRecordSet* ohlcRecordSet, int period )
{
	int lastIdx = ohlcRecordSet->GetEndIndex();
	int beginIdx = ohlcRecordSet->GetBeginIndex();
	int loopEndIdx = lastIdx - period + 1;
	loopEndIdx = beginIdx > loopEndIdx ? beginIdx : loopEndIdx;
	double totalRange = 0.0;
	int actCount = 0;
	for(int idx = lastIdx; idx >= loopEndIdx; --idx, ++actCount)
	{
		totalRange += ohlcRecordSet->HighSeries[idx] - ohlcRecordSet->LowSeries[idx];
	}
	double avgRange = actCount > 0 ? totalRange / actCount : 0;
	return avgRange;
}

int CWillRDataSet::TestMRO1( COHLCRecordSet* ohlcRecordSet, int period, double avgRange )
{
	int lastIdx = ohlcRecordSet->GetEndIndex();
	int beginIdx = ohlcRecordSet->GetBeginIndex();

	int currentBar = lastIdx;
	int mroPos = 0;
	while(currentBar > beginIdx && currentBar > lastIdx - period)
	{
		double diff = ohlcRecordSet->OpenSeries[currentBar] - ohlcRecordSet->CloseSeries[currentBar - 1];
		if(fabs(diff) >= avgRange * 2.0)
			return mroPos;

		++mroPos;
		--currentBar;
	}

	return period + 1;
}

bool CWillRDataSet::TestMRO2( COHLCRecordSet* ohlcRecordSet, int period, double avgRange )
{
	int lastIdx = ohlcRecordSet->GetEndIndex();
	int beginIdx = ohlcRecordSet->GetBeginIndex();

	int currentBar = lastIdx;
	while(currentBar > beginIdx && currentBar > lastIdx - period)
	{
		double diff = ohlcRecordSet->CloseSeries[currentBar] - ohlcRecordSet->CloseSeries[currentBar - 3];
		if(fabs(diff) >= avgRange * 4.6)
			return true;

		--currentBar;
	}

	return false;
}


