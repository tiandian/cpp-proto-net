#pragma once

#include "TaIndicatorSet.h"

#define IND_WR "WR"

class CWillRDataSet : public CTaIndicatorSet
{
public:
	CWillRDataSet(int size, int risk);
	~CWillRDataSet(void);

	void Calculate(COHLCRecordSet* ohlcRecordSet);

	void SetRisk(int val){ m_risk = val; }
	int GetRisk(){ return m_risk; }

private:
	
	int m_risk;
	double* m_arrWR;
};

typedef boost::shared_ptr<CWillRDataSet> WillRDataSetPtr;
