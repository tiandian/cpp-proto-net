#pragma once

class COHLCRecordSet;
class CTaIndicatorSet;

class CTaCalculator
{
public:
	CTaCalculator(void);
	virtual ~CTaCalculator(void);

	virtual void Calc(COHLCRecordSet* ohlcRecordSet, CTaIndicatorSet* taSet) = 0;
};

class CMACDCalculator : public CTaCalculator
{
public:
	CMACDCalculator(){}
	~CMACDCalculator(){}

	void Calc(COHLCRecordSet* ohlcRecordSet, CTaIndicatorSet* taSet);
};

typedef boost::shared_ptr<CTaCalculator> TaCalculatorPtr;

