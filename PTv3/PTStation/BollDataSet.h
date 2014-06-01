#pragma once

#include "TaIndicatorSet.h"

#define IND_MID "MID"
#define IND_TOP "TOP"
#define IND_BOTTOM "BOTTOM"

class CDiffRecordSet;

class CBollDataSet : public CTaIndicatorSet
{
public:
	CBollDataSet(int size, int paramM, int paramP);
	~CBollDataSet(void);

	void SetM(int val){ m_paramM = val; }
	int GetM(){ return m_paramM; }

	void SetP(int val){ m_paramP = val; }
	int GetP(){ return m_paramP; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);
	void Calculate(CDiffRecordSet* diffRecordSet);
private:

	void CalculateRaw(double* pxArray, int lastIdx);
	void CalculateTA(double* pxArray, int lastIdx);

	int m_paramM;
	int m_paramP;

	double* m_arrMid;
	double* m_arrTop;
	double* m_arrBottom;
};

typedef boost::shared_ptr<CBollDataSet> BollDataSetPtr;