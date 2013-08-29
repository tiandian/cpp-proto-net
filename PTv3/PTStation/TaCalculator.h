#pragma once

#include "TaIndicatorSet.h"

class COHLCRecordSet;

class CTaCalculator
{
public:
	CTaCalculator(void);
	virtual ~CTaCalculator(void);

	virtual void Calc(COHLCRecordSet* ohlcRecordSet) = 0;
	
	CTaIndicatorSet* GetTaSet(){ return m_taSet.get(); }

private:
	TaIndicatorSetPtr m_taSet;
};

class CMACDCalculator : public CTaCalculator
{
public:
	CMACDCalculator(){}
	~CMACDCalculator(){}

	void Calc(COHLCRecordSet* ohlcRecordSet);
};

typedef boost::shared_ptr<CTaCalculator> TaCalculatorPtr;

