#pragma once

#include "TaIndicatorSet.h"

#define IND_ATR "DonchianHi"
#define IND_TRUE_RANGE "TRange"

class CATRDataSet : public CTaIndicatorSet
{
public:
	CATRDataSet(int size, int period);
	~CATRDataSet(void);

	void SetPeriod(int val){ m_period = val; }
	int GetPeriod(){ return m_period; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

private:
	void CalcTrueRange(COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx);
	void CalcATR(COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx);

	int m_period;

	double* m_arrTrueRange;
	double* m_arrATR;
};

typedef boost::shared_ptr<CATRDataSet> ATRDataSetPtr;
