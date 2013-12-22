#pragma once

#include "TaIndicatorSet.h"

#define IND_WR "WR"
#define IND_Donchian_Hi "DonchianHi"
#define IND_Donchian_Lo "DonchianLo"

enum { 
	ASC_X1 = 67,
	ASC_X2 = 33
};

class CWillRDataSet : public CTaIndicatorSet
{
public:
	CWillRDataSet(int size, int risk, int atrPeriod, int breakoutLength);
	~CWillRDataSet(void);

	void Calculate(COHLCRecordSet* ohlcRecordSet);

	void SetRisk(int val){ m_risk = val; }
	int Risk(){ return m_risk; }

	int AtrPeriod() const { return m_atrPeriod; }
	void SetAtrPeriod(int val) { m_atrPeriod = val; }

	int BreakoutLength() const { return m_breakoutLength; }
	void SetBreakoutLength(int val) { m_breakoutLength = val; }

private:

	void CalcWilliamsR(COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx);
	void CalcDonchianChannel(COHLCRecordSet* ohlcRecordSet, int nbElements, int lastIdx);
	
	double GetATR(COHLCRecordSet* ohlcRecordSet, int period);
	// See if there is a bar whose open is 2 times atr greater/less than previous close 
	bool TestMRO1(COHLCRecordSet* ohlcRecordSet, int period, double avgRange);
	// See if there is a bar whose close is 4.6 times atr greater/less than the last 3 bar
	bool TestMRO2(COHLCRecordSet* ohlcRecordSet, int period, double avgRange);

	int m_risk;
	int m_atrPeriod;
	int m_breakoutLength;
	
	double* m_arrWR;
	double* m_arrHi;
	double* m_arrLo;

};

typedef boost::shared_ptr<CWillRDataSet> WillRDataSetPtr;
