#pragma once

#include "TaIndicatorSet.h"

#define IND_WATR_TREND "WATR_Trend"
#define IND_WATR_STOP "WATR_Stop"
#define IND_WATR_SUMMIT_PRICE "Summit_Px"
#define IND_WATR_VAL "WATR_Val"

class CWATRStopDataSet : public CTaIndicatorSet
{
public:
	CWATRStopDataSet(int size, int atrPeriod);
	~CWATRStopDataSet(void);

	int AtrPeriod() const { return m_atrPeriod; }
	void SetAtrPeriod(int val) { m_atrPeriod = val; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

private:

	void CalcWATR( COHLCRecordSet* ohlcRecordSet, int idx );
	void CalcWATRStop(int beginIdx, int endIdx, COHLCRecordSet* ohlcRecordSet, int* outBeg, int* outNbElem);
	void CalcWATRStopAt(COHLCRecordSet* ohlcRecordSet, int currIdx);
	void CalcWATRStopLast(COHLCRecordSet* ohlcRecordSet);

	int m_atrPeriod;

	double* m_arrTrend;
	double* m_arrSummitPx;
	double* m_arrStopPx;
	double* m_arrWatr;
};

typedef boost::shared_ptr<CWATRStopDataSet> WATRStopDataSetPtr;
