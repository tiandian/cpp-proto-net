#pragma once

#include "DiffStrategy.h"

class CArbitrageStrategy : public CDiffStrategy
{
public:
	CArbitrageStrategy(void);
	~CArbitrageStrategy(void);

	void SetOpenPosiCond(COMPARE_OP op, double targetVal)
	{
		m_openPosiChecker.Comparsion(op);
		m_openPosiChecker.TargetVal(targetVal);
	}

	void SetStopGainCond(COMPARE_OP op, double targetVal)
	{
		m_stopGainChecker.Comparsion(op);
		m_stopGainChecker.TargetVal(targetVal);
	}

	void SetStopLossCond(COMPARE_OP op, double targetVal)
	{
		m_stopLossChecker.Comparsion(op);
		m_stopLossChecker.TargetVal(targetVal);
	}

	void ApplySettings(const std::string& settingData);

protected:

	CConditionChecker<double>& GetOpenPosiCond(){ return m_openPosiChecker; }
	CConditionChecker<double>& GetStopGainCond(){ return m_stopGainChecker; }
	CConditionChecker<double>& GetStopLossCond(){ return m_stopLossChecker; }

	void DoOpenPostion();
	void DoStopGain(){}
	void DoStopLoss(){}

private:
	CValueChecker m_openPosiChecker;
	CValueChecker m_stopGainChecker;
	CValueChecker m_stopLossChecker;
};

