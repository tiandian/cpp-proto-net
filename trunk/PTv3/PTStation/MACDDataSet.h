#pragma once

#include "TaIndicatorSet.h"

#define IND_MACD "MACD"
#define IND_MACD_SIGNAL "MACDSignal"
#define IND_MACD_HIST "MACDHist"

class CMACDDataSet : public CTaIndicatorSet
{
public:
	CMACDDataSet(unsigned int size, double paramShort, double paramLong, double paramM);
	~CMACDDataSet(void);

	void SetShort(double val){ m_paramShort = val; }
	double GetShort(){ return m_paramShort; }

	void SetLong(double val){ m_paramLong = val; }
	double GetLong(){ return m_paramLong; }

	void SetM(double val){ m_paramM = val; }
	double GetM(){ return m_paramM; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

private:
	double m_paramShort;
	double m_paramLong;
	double m_paramM;

	double* m_arrMacd;
	double* m_arrMacdSignal;
	double* m_arrMacdHist;
};

typedef boost::shared_ptr<CMACDDataSet> MACDDataSetPtr;

