#include "StdAfx.h"
#include "DonchianDataSet.h"
#include "OHLCRecordSet.h"
#include "globalmembers.h"

CDonchianDataSet::CDonchianDataSet(int size, int period)
	: CTaIndicatorSet(size)
{
	SetPeriod(period);

	m_arrHighest = AddIndicator(IND_DONCHIAN_Hi);
	m_arrLowest = AddIndicator(IND_DONCHIAN_Lo);
}


CDonchianDataSet::~CDonchianDataSet(void)
{
}

void CDonchianDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	int nbElements = ohlcRecordSet->NbElements();
	int lastIdx = ohlcRecordSet->GetEndIndex();
	logger.Info(boost::str(boost::format("Calculating Donchian with OHLC RecordSet: lastIdx - %d, last price - %f")
		% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));

	CalcDonchianChannel(ohlcRecordSet, nbElements, lastIdx);

	m_lastPosition = lastIdx;
}

void CDonchianDataSet::CalcDonchianChannel( COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx )
{
	if(nbElements >= m_period)
	{
		int startIdx = lastIdx;
		int endIdx = lastIdx - m_period + 1;
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

		m_arrHighest[lastIdx] = topBound;
		m_arrLowest[lastIdx] = bottomBound;

		logger.Info(boost::str(boost::format("Calculated Donchian Channel: High - %.2f, Low - %.2f")
			% m_arrHighest[lastIdx] % m_arrLowest[lastIdx]));
	}
}
