#pragma once

#include "TaIndicatorSet.h"

#define IND_MACD "MACD"
#define IND_MACD_SIGNAL "MACDSignal"
#define IND_MACD_HIST "MACDHist"

enum SEED_CALC_METHOD
{
	CLASSIC_MA,
	INPUT_SEED
};

class CMACDDataSet : public CTaIndicatorSet
{
public:
	CMACDDataSet(int size, int paramShort, int paramLong, int paramM);
	~CMACDDataSet(void);

	void SetShort(int val);
	int GetShort(){ return m_paramShort; }

	void SetLong(int val);
	int GetLong(){ return m_paramLong; }

	void SetM(int val);
	int GetM(){ return m_paramM; }

	void Calculate(COHLCRecordSet* ohlcRecordSet);

	void SetSeedCalcMethod(SEED_CALC_METHOD method){ m_seedCalcMethod = method; }
	SEED_CALC_METHOD SeedCalcMethod(){ return m_seedCalcMethod; }
	
	void SeedShort(double val){ m_seedShort = val;}
	double SeedShort(){ return m_seedShort; }
	void SeedLong(double val){ m_seedLong = val;}
	double SeedLong(){ return m_seedLong; }
	void SeedSignal(double val){ m_seedSignal = val;}
	double SeedSignal(){ return m_seedSignal; }

	void ResetSeedPosition();

private:
	void CalculateBaseOnHistData( COHLCRecordSet* ohlcRecordSet );
	void CalculateBaseOnSeed( COHLCRecordSet* ohlcRecordSet );

	void CalculateMACD( int beginIdx, int endIdx, COHLCRecordSet* ohlcRecordSet );

	void ResetSeed(int seedPosition);

	int m_paramShort;
	int m_paramLong;
	int m_paramM;

	double m_ShortEmaFactor;
	double m_LongEmaFactor;
	double m_SignalEmaFactor;

	double* m_arrMacd;
	double* m_arrMacdSignal;
	double* m_arrMacdHist;

	DataSeries m_arrEmaShort;
	DataSeries m_arrEmaLong;

	SEED_CALC_METHOD m_seedCalcMethod;

	double m_seedShort;
	double m_seedLong;
	double m_seedSignal;

	bool m_resettingSeedPosition;
};

typedef boost::shared_ptr<CMACDDataSet> MACDDataSetPtr;

