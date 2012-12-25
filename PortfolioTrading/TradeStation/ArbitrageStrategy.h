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
		m_openPosiChecker.SetTargetVal(targetVal);
	}

	void SetStopGainCond(COMPARE_OP op, double targetVal)
	{
		m_stopGainChecker.Comparsion(op);
		m_stopGainChecker.SetTargetVal(targetVal);
		m_stopGainChecker.SetGetCostFunc(boost::bind(&CPortfolio::AvgCost, Portfolio()));
	}

	void SetStopLossCond(COMPARE_OP op, double targetVal)
	{
		m_stopLossChecker.Comparsion(op);
		m_stopLossChecker.SetTargetVal(targetVal);
		m_stopLossChecker.SetGetCostFunc(boost::bind(&CPortfolio::AvgCost, Portfolio()));
	}

	void ApplySettings(const std::string& settingData);

	void Test();

protected:

	CConditionChecker<double>& GetOpenPosiCond(){ return m_openPosiChecker; }
	CConditionChecker<double>& GetStopGainCond(){ return m_stopGainChecker; }
	CConditionChecker<double>& GetStopLossCond(){ return m_stopLossChecker; }

	void DoOpenPostion();
	void DoStopGain(){ ClosePosition(trade::SR_StopGain); }
	void DoStopLoss(){ ClosePosition(trade::SR_StopLoss); }

	POSI_OPER NextOperation(POSI_OPER oper);

private:

	void ClosePosition(trade::SubmitReason submitReason);

	CValueChecker m_openPosiChecker;
	CCostValueChecker m_stopGainChecker;
	CCostValueChecker m_stopLossChecker;
};

