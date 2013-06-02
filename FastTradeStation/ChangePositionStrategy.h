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
		m_openPosiChecker.SetTargetVal(targetVal);
	}

	void ApplySettings(const std::string& settingData);

	void Test();

protected:

	CConditionChecker<double>& GetOpenPosiCond(){ return m_openPosiChecker; }
	CConditionChecker<double>& GetStopGainCond(){ return m_stopGainChecker; }
	CConditionChecker<double>& GetStopLossCond(){ return m_stopLossChecker; }

	void DoOpenPostion();
	
	POSI_OPER NextOperation(POSI_OPER oper);

private:
	CValueChecker m_openPosiChecker;
	CNonUseChecker m_stopGainChecker;
	CNonUseChecker m_stopLossChecker;

	std::string m_closeLeg;
	entity::PosiDirectionType m_closingLegPosition;
};

