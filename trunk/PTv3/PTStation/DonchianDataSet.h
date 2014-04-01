#pragma once

#include "TaIndicatorSet.h"

#define IND_DONCHIAN_Hi "DonchianHi"
#define IND_DONCHIAN_Lo "DonchianLo"

class CDonchianDataSet : public CTaIndicatorSet
{
public:
	CDonchianDataSet(int size, int period);
	~CDonchianDataSet(void);

	void SetPeriod(int val){ m_period = val; }
	int GetPeriod(){ return m_period; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

private:
	void CalcDonchianChannel(COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx);

	int m_period;

	double* m_arrHighest;
	double* m_arrLowest;
};

typedef boost::shared_ptr<CDonchianDataSet> DonchianDataSetPtr;

