#include "StdAfx.h"
#include "Strategy.h"

bool DoubleGreaterEqual(double a, double b)
{
	if(a >= b) return true;
	else
		return b - a < DOUBLE_COMPARSION_PRECISION; 
}

bool DoubleLessEqual(double a, double b)
{
	return DoubleGreaterEqual(b, a);
}

bool DoubleEqual(double a, double b)
{
	double diff = a - b;
	return diff < DOUBLE_COMPARSION_PRECISION && diff > -DOUBLE_COMPARSION_PRECISION;
}

COMPARE_OP ConvertCompareCondition(entity::CompareCondition compareCond)
{
	return (COMPARE_OP)((int)compareCond + 1);
}

const char* ST_OPEN_POSI = "Auto Open Position";
const char* ST_CLOSE_POSI = "Auto Close Position";
const char* ST_DO_NOTHING = "Do nothing";

const char* StrategyOpertaionText(POSI_OPER oper)
{
	if(oper == OPEN_POSI) return ST_OPEN_POSI;
	else if(oper == CLOSE_POSI) return ST_CLOSE_POSI;
	else return ST_DO_NOTHING;
}

COMPARE_OP ReverseCondition(COMPARE_OP compareCond)
{
	switch(compareCond) 
	{
	case GREATER_THAN:
		return LESS_THAN;
	case GREATER_EQUAL_THAN:
		return LESS_EQUAL_THAN;
	case LESS_THAN:
		return GREATER_THAN;
	case LESS_EQUAL_THAN:
		return GREATER_EQUAL_THAN;
	default:
		return NOT_COMPARE;
	}
}
