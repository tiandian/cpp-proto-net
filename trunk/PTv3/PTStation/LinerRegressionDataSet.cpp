#include "StdAfx.h"
#include "LinerRegressionDataSet.h"
#include "globalmembers.h"
#include "OHLCRecordSet.h"

#include <ta-lib/ta_libc.h>

CLinerRegressionDataSet::CLinerRegressionDataSet(int size, int period)
	: CTaIndicatorSet(size)
	, m_paramPeriod(period)
{
	m_arrAngle = AddIndicator(IND_LREG, INVALID_LREG_ANGLE);
}


CLinerRegressionDataSet::~CLinerRegressionDataSet(void)
{
}

void CLinerRegressionDataSet::Calculate( COHLCRecordSet* ohlcRecordSet )
{
	int outBeg = -1;
	int outNbElement = -1;
	double outAngle = 0;

	int nonBlankCount = ohlcRecordSet->NbElements();
	int lastIdx = ohlcRecordSet->GetEndIndex();
	logger.Info(boost::str(boost::format("Calculating Liner Regression Angle with OHLC RecordSet: lastIdx - %d, weight avg - %f, nbElements - %d")
		% lastIdx % (ohlcRecordSet->WeightAvgSeries)[lastIdx] % nonBlankCount));

	TA_RetCode rc = TA_LINEARREG_ANGLE(lastIdx, lastIdx, (ohlcRecordSet->WeightAvgSeries).get(), m_paramPeriod, &outBeg, &outNbElement, &outAngle);

	if(outBeg == lastIdx && nonBlankCount > 2)
	{
		m_arrAngle[outBeg] = outAngle;

		logger.Info(boost::str(boost::format("Calculated Liner Regression Angle: %.2f")
			% m_arrAngle[outBeg]));
	}

	m_lastPosition = lastIdx;
}

bool CLinerRegressionDataSet::IsAngleValid( double angle )
{
	if(angle > LREG_ANGLE_BOUNDERY || angle < -LREG_ANGLE_BOUNDERY)
		return false;

	return true;
}
