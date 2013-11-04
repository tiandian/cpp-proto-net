#pragma once

#include "TaIndicatorSet.h"

#define IND_LREG "LREG"
#define INVALID_LREG_ANGLE ((double)180)
#define LREG_ANGLE_BOUNDERY ((double)90)

class CLinerRegressionDataSet : public CTaIndicatorSet
{
public:
	CLinerRegressionDataSet(int size, int period);
	~CLinerRegressionDataSet(void);

	void SetPeriod(int val){ m_paramPeriod = val; }
	int GetPeriod(){ return m_paramPeriod; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

	static bool IsAngleValid(double angle);

private:
	int m_paramPeriod;

	double* m_arrAngle;
};

typedef boost::shared_ptr<CLinerRegressionDataSet> LinerRegSetPtr;
