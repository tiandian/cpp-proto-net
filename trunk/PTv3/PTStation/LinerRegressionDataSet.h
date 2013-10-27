#pragma once

#include "TaIndicatorSet.h"

#define IND_LREG "LREG"

class CLinerRegressionDataSet : public CTaIndicatorSet
{
public:
	CLinerRegressionDataSet(int size, int period);
	~CLinerRegressionDataSet(void);

	void SetPeriod(int val){ m_paramPeriod = val; }
	int GetPeriod(){ return m_paramPeriod; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

private:
	int m_paramPeriod;

	double* m_arrAngle;
};

typedef boost::shared_ptr<CLinerRegressionDataSet> LinerRegSetPtr;
