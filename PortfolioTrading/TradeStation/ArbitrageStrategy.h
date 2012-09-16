#pragma once

#include "Strategy.h"

#define DOUBLE_COMPARSION_PRECISION (0.01)

enum COMPARE_OP
{
	NOT_COMPARE,
	GREATER_THAN,
	GREATER_EQUAL_THAN,
	LESS_THAN,
	LESS_EQUAL_THAN
};

bool DoubleGreaterEqual(double a, double b);

class CValueChecker : public CConditionChecker<double>
{
public:
	CValueChecker() : m_comparsion(NOT_COMPARE), m_targetVal(0)
	{
	}

	double TargetVal() const { return m_targetVal; }
	void TargetVal(double val) { m_targetVal = val; }

	COMPARE_OP Comparsion() const { return m_comparsion; }
	void Comparsion(COMPARE_OP val) { m_comparsion = val; }

	bool Test(double valToTest)
	{
		switch(m_comparsion)
		{
		case GREATER_THAN:
			return valToTest > m_targetVal;
		case GREATER_EQUAL_THAN:
			return DoubleGreaterEqual(valToTest, m_targetVal);
		case LESS_THAN:
			return valToTest < m_targetVal;
		case LESS_EQUAL_THAN:
			return DoubleGreaterEqual(m_targetVal, valToTest);
		default:
			return false;
		}
		return false; 
	}

private:
	double m_targetVal;
	COMPARE_OP m_comparsion;	
	
};

class CArbitrageStrategy : public CStrategy<double>
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

protected:

	CConditionChecker<double>& GetOpenPosiCond(){ return m_openPosiChecker; }
	CConditionChecker<double>& GetStopGainCond(){ return m_stopGainChecker; }
	CConditionChecker<double>& GetStopLossCond(){ return m_stopLossChecker; }

private:
	CValueChecker m_openPosiChecker;
	CValueChecker m_stopGainChecker;
	CValueChecker m_stopLossChecker;
};

