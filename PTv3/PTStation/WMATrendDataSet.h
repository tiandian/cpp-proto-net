#pragma once

#include "TaIndicatorSet.h"

#define IND_FAST_LINE "FAST_LINE"
#define IND_SLOW_LINE "SLOW_LINE"

class CWMATrendDataSet : public CTaIndicatorSet
{
public:
	CWMATrendDataSet(int size, int wmaParam, int maN);
	~CWMATrendDataSet(void);

	void SetWmaParam(int val){ m_wmaParam = val; }
	int GetWmaParam(){ return m_wmaParam; }

	void SetMaN(int val){ m_maN = val; }
	int GetMaN(){ return m_maN; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

private:

	int m_wmaParam;
	int m_maN;

	double* m_arrFast;
	double* m_arrSlow;
};

typedef boost::shared_ptr<CWMATrendDataSet> WMATrendSetPtr;

