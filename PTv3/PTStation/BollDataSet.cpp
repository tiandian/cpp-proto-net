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
	if(lastIdx < 0) return;

	logger.Info(boost::str(boost::format("Calculating BOLL with OHLC RecordSet: lastIdx - %d, last price - %f")
		% lastIdx % (ohlcRecordSet->CloseSeries)[lastIdx]));

	//CalculateTA(ohlcRecordSet, lastIdx);
	CalculateRaw((ohlcRecordSet->CloseSeries).get(), lastIdx);
}

void CBollDataSet::Calculate( CDiffRecordSet* diffRecordSet )
{
	int lastIdx = diffRecordSet->GetEndIndex();
	if(lastIdx < 0) return;

	logger.Info(boost::str(boost::format("Calculating BOLL with DiffRecordSet: lastIdx - %d, last price - %f")
		% lastIdx % (diffRecordSet->DiffSeries)[lastIdx]));

	//CalculateTA((diffRecordSet->DiffSeries).get(), lastIdx);
	CalculateRaw((diffRecordSet->DiffSeries).get(), lastIdx);
}

void CBollDataSet::CalculateRaw( double* pxArray, int lastIdx )
{
	int outBeg = -1;
	int outNbElement = -1;
	double outSma = 0;
	double outStdDev = 0;

	if(m_lastPosition > 0)
	{
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
	else
	{
		int sma_lookback = TA_SMA_Lookback(m_paramM);
		TA_RetCode rc = TA_SMA(0, lastIdx, pxArray, m_paramM, &outBeg, &outNbElement,
			m_arrMid + sma_lookback);
		if(outBeg == sma_lookback)
		{
			int dev_lookback = TA_STDDEV_Lookback(m_paramM, (double)m_paramP);
			DataSeries stdDevSeries(new double[m_size]);
			ResetArray(stdDevSeries.get(), m_size, 0);
			TA_RetCode rc = TA_STDDEV(0, lastIdx, pxArray, m_paramM, (double)m_paramP, &outBeg, &outNbElement,
				stdDevSeries.get() + dev_lookback);
			if(outBeg != dev_lookback)
				logger.Warning(boost::str(boost::format("Calculated MA on WMA line may NOT be correct!!!")));
			else
			{
				for(int i = 0; i < lastIdx; ++i)
				{
					m_arrTop[i] = m_arrMid[i] + stdDevSeries[i];
					m_arrBottom[i] = m_arrMid[i] - stdDevSeries[i];
				}
			}
		}
	}

	m_lastPosition = lastIdx;
}

void CBollDataSet::CalculateTA( double* pxArray, int lastIdx )
{
	int outBeg = -1;
	int outNbElement = -1;
	double outMid = 0;
	double outTop = 0;
	double outBottom = 0;

	if(m_lastPosition > 0)
	{
		TA_RetCode rc = TA_BBANDS(lastIdx, lastIdx, pxArray, 
			m_paramM, (double)m_paramP, (double)m_paramP, TA_MAType_SMA, &outBeg, &outNbElement, &outTop, &outMid, &outBottom);

		if(outBeg == lastIdx)
		{
			m_arrTop[outBeg] = outTop;
			m_arrBottom[outBeg] = outBottom;
			m_arrMid[outBeg] = outMid;

			logger.Info(boost::str(boost::format("Calculated BOLL values: mid - %.2f, top - %.2f, bottom - %.2f")
				% m_arrMid[outBeg] % m_arrTop[outBeg] % m_arrBottom[outBeg] ));
		}
	}
	else
	{
		int boll_lookback = TA_BBANDS_Lookback(m_paramM, (double)m_paramP, (double)m_paramP, TA_MAType_SMA);
		TA_RetCode rc = TA_BBANDS(0, lastIdx, pxArray, m_paramM, (double)m_paramP, (double)m_paramP, TA_MAType_SMA, 
			&outBeg, &outNbElement, m_arrTop + boll_lookback, m_arrMid + boll_lookback, m_arrBottom + boll_lookback);
		if(outBeg != boll_lookback)
		{
			logger.Warning(boost::str(boost::format("Calculated Boll Band may NOT be correct!!!")));
		}
	}

	m_lastPosition = lastIdx;
}
