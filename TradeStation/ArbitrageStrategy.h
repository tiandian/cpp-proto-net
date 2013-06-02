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
		if(Direction() == entity::LONG)
		{
			m_stopGainChecker.Comparsion(op);
		}
		else
		{
			m_stopGainChecker.Comparsion(ReverseCondition(op));
		}
		m_stopGainChecker.SetTargetVal(targetVal);
		m_stopGainChecker.SetGetCostFunc(boost::bind(&CPortfolio::AvgCost, Portfolio()));
	}

	void SetStopLossCond(COMPARE_OP op, double targetVal)
	{
		if(Direction() == entity::LONG)
		{
			m_stopLossChecker.Comparsion(op);
		}
		else
		{
			m_stopLossChecker.Comparsion(ReverseCondition(op));
		}
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
	CGainCostValueChecker m_stopGainChecker;
	CLossCostValueChecker m_stopLossChecker;
};

