#pragma once
#include "DiffStrategy.h"

class CChangePositionStrategy : public CDiffStrategy
{
public:
	CChangePositionStrategy(void);
	~CChangePositionStrategy(void);

	std::string CloseLeg() const { return m_closeLeg; }
	void CloseLeg(std::string val) { m_closeLeg = val; }

	void SetOpenPosiCond(COMPARE_OP op, double targetVal)
	{
		m_openPosiChecker.Comparsion(op);
		m_openPosiChecker.TargetVal(targetVal);
	}

	void ApplySettings(const std::string& settingData);

protected:

	CConditionChecker<double>& GetOpenPosiCond(){ return m_openPosiChecker; }
	CConditionChecker<double>& GetStopGainCond(){ return m_stopGainChecker; }
	CConditionChecker<double>& GetStopLossCond(){ return m_stopLossChecker; }

private:
	CValueChecker m_openPosiChecker;
	CNonUseChecker m_stopGainChecker;
	CNonUseChecker m_stopLossChecker;

	std::string m_closeLeg;

};

