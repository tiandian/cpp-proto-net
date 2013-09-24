#pragma once

#include "TaIndicatorSet.h"

#define IND_MACD "MACD"
#define IND_MACD_SIGNAL "MACDSignal"
#define IND_MACD_HIST "MACDHist"

class CMACDDataSet : public CTaIndicatorSet
{
public:
	CMACDDataSet(int size, int paramShort, int paramLong, int paramM);
	~CMACDDataSet(void);

	void SetShort(int val){ m_paramShort = val; }
	int GetShort(){ return m_paramShort; }

	void SetLong(int val){ m_paramLong = val; }
	int GetLong(){ return m_paramLong; }

	void SetM(int val){ m_paramM = val; }
	int GetM(){ return m_paramM; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

private:
	int m_paramShort;
	int m_paramLong;
	int m_paramM;

	double* m_arrMacd;
	double* m_arrMacdSignal;
	double* m_arrMacdHist;
};

typedef boost::shared_ptr<CMACDDataSet> MACDDataSetPtr;

