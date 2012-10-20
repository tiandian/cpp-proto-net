#pragma once

#include "Strategy.h"
#include "globalmembers.h"

#include <boost/format.hpp>

class CNonUseChecker : public CConditionChecker<double>
{
public:
	bool Test(double val) { return false; }
};

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
			logger.Info(boost::str(boost::format("? %f > %f")
				% valToTest % m_targetVal));
			return valToTest > m_targetVal;
		case GREATER_EQUAL_THAN:
			logger.Info(boost::str(boost::format("? %f >= %f")
				% valToTest % m_targetVal));
			return DoubleGreaterEqual(valToTest, m_targetVal);
		case LESS_THAN:
			logger.Info(boost::str(boost::format("? %f < %f")
				% valToTest % m_targetVal));
			return valToTest < m_targetVal;
		case LESS_EQUAL_THAN:
			logger.Info(boost::str(boost::format("? %f <= %f")
				% valToTest % m_targetVal));
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

class CDiffStrategy : public CStrategy<double>
{
public:
	virtual ~CDiffStrategy(){}
};